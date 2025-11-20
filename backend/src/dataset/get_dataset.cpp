#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class CelebADatasetPreparer
{
public:
  struct Sample
  {
    std::string image_path;
    int label;
  };

  CelebADatasetPreparer(std::string images_root,
                        std::string attr_file,
                        std::string path_prefix_for_output = "")
      : images_root_(std::move(images_root)),
        attr_file_(std::move(attr_file)),
        path_prefix_(std::move(path_prefix_for_output)),
        rng_(std::random_device{}())
  {
  }

  void loadAttributes()
  {
    parseAttributeFile();
  }

  void createBalancedDataset(std::size_t max_total = 35000)
  {
    if (all_meta_.empty())
    {
      throw std::runtime_error("Attributes not loaded. Call loadAttributes() first.");
    }

    const auto idx_receding = getAttrIndex("Receding_Hairline");
    const auto idx_young = getAttrIndex("Young");
    const auto idx_attr = getAttrIndex("Attractive");

    std::vector<const Meta *> pos_candidates;
    std::vector<const Meta *> neg_candidates;

    for (const auto &m : all_meta_)
    {
      bool receding = m.attrs[idx_receding] == 1;
      if (receding)
      {
        pos_candidates.push_back(&m);
      }
      else
      {
        neg_candidates.push_back(&m);
      }
    }

    if (pos_candidates.empty())
    {
      throw std::runtime_error("No positive (Receding_Hairline=1, Male=1) samples found.");
    }
    if (neg_candidates.empty())
    {
      throw std::runtime_error("No negative (Receding_Hairline=-1, Male=1) samples found.");
    }

    std::shuffle(pos_candidates.begin(), pos_candidates.end(), rng_);

    std::size_t P = pos_candidates.size();
    std::size_t N = neg_candidates.size();
    std::size_t per_class = std::min<std::size_t>({P, N, max_total / 2});

    if (per_class == 0)
    {
      throw std::runtime_error("per_class == 0. Not enough data or max_total too small.");
    }

    std::vector<const Meta *> selected_pos(
        pos_candidates.begin(),
        pos_candidates.begin() + per_class);

    std::array<std::vector<const Meta *>, 4> strata;
    for (const auto *m : neg_candidates)
    {
      bool young = m->attrs[idx_young] == 1;
      bool attractive = m->attrs[idx_attr] == 1;
      int key = (young ? 1 : 0) + (attractive ? 2 : 0);
      strata[(std::size_t)key].push_back(m);
    }

    for (auto &bucket : strata)
    {
      std::shuffle(bucket.begin(), bucket.end(), rng_);
    }

    std::vector<const Meta *> selected_neg;
    selected_neg.reserve(per_class);

    std::size_t base_quota = per_class / 4;
    std::size_t taken_total = 0;

    for (std::size_t i = 0; i < 4; ++i)
    {
      auto &bucket = strata[i];
      std::size_t take = std::min(base_quota, bucket.size());
      selected_neg.insert(selected_neg.end(), bucket.begin(), bucket.begin() + take);
      taken_total += take;
    }

    if (taken_total < per_class)
    {
      std::size_t remaining = per_class - taken_total;
      for (std::size_t i = 0; i < 4 && remaining > 0; ++i)
      {
        auto &bucket = strata[i];
        std::size_t already_taken = std::min(base_quota, bucket.size());
        if (already_taken < bucket.size())
        {
          std::size_t can_take = std::min(bucket.size() - already_taken, remaining);
          selected_neg.insert(
              selected_neg.end(),
              bucket.begin() + (std::ptrdiff_t)already_taken,
              bucket.begin() + (std::ptrdiff_t)(already_taken + can_take));
          remaining -= can_take;
        }
      }
    }

    if (selected_neg.size() < per_class)
    {
      std::cerr << "Warning: could only select " << selected_neg.size()
                << " negatives (requested " << per_class << ").\n";
    }

    balanced_samples_.clear();
    balanced_samples_.reserve(selected_pos.size() + selected_neg.size());

    for (const auto *m : selected_pos)
    {
      balanced_samples_.push_back(Sample{makeImagePath(m->filename), 1});
    }
    for (const auto *m : selected_neg)
    {
      balanced_samples_.push_back(Sample{makeImagePath(m->filename), 0});
    }

    std::shuffle(balanced_samples_.begin(), balanced_samples_.end(), rng_);
  }

  const std::vector<Sample> &samples() const
  {
    return balanced_samples_;
  }

  void saveSplits(const std::string &out_dir,
                  double train_frac = 0.8,
                  double val_frac = 0.1) const
  {
    if (balanced_samples_.empty())
    {
      throw std::runtime_error("No samples in balanced dataset. Call createBalancedDataset() first.");
    }

    if (train_frac <= 0.0 || val_frac < 0.0 || train_frac + val_frac >= 1.0)
    {
      throw std::invalid_argument("Invalid train/val fractions.");
    }

    std::size_t total = balanced_samples_.size();
    std::size_t n_train = static_cast<std::size_t>(total * train_frac);
    std::size_t n_val = static_cast<std::size_t>(total * val_frac);
    std::size_t n_test = total - n_train - n_val;

    auto write_split = [&](const std::string &filename,
                           std::size_t offset,
                           std::size_t count)
    {
      std::ofstream out(out_dir + "/" + filename);
      if (!out)
      {
        throw std::runtime_error("Could not open output file: " + out_dir + "/" + filename);
      }
      for (std::size_t i = 0; i < count; ++i)
      {
        const auto &s = balanced_samples_[offset + i];
        out << s.image_path << " " << s.label << "\n";
      }
    };

    write_split("train.txt", 0, n_train);
    write_split("val.txt", n_train, n_val);
    write_split("test.txt", n_train + n_val, n_test);
  }

private:
  struct Meta
  {
    std::string filename;
    std::vector<int8_t> attrs;
  };

  std::string images_root_;
  std::string attr_file_;
  std::string path_prefix_;

  std::vector<Meta> all_meta_;
  std::unordered_map<std::string, std::size_t> attr_index_;
  std::size_t num_attrs_ = 0;

  std::vector<Sample> balanced_samples_;
  std::mt19937 rng_;

  std::size_t getAttrIndex(const std::string &name) const
  {
    auto it = attr_index_.find(name);
    if (it == attr_index_.end())
    {
      throw std::runtime_error("Attribute not found: " + name);
    }
    return it->second;
  }

  std::string makeImagePath(const std::string &filename) const
  {
    if (images_root_.empty())
      return filename;
    if (images_root_.back() == '/' || images_root_.back() == '\\')
    {
      return images_root_ + filename;
    }
    return images_root_ + "/" + filename;
  }

  static bool isInteger(const std::string &s)
  {
    if (s.empty())
      return false;
    for (char c : s)
    {
      if (!std::isdigit(static_cast<unsigned char>(c)))
        return false;
    }
    return true;
  }

  static std::vector<std::string> splitComma(const std::string &line)
  {
    std::vector<std::string> tokens;
    std::string cur;
    std::istringstream iss(line);
    while (!iss.eof())
    {
      if (!std::getline(iss, cur, ','))
        break;
      auto start = cur.find_first_not_of(" \t\r\n");
      auto end = cur.find_last_not_of(" \t\r\n");
      if (start == std::string::npos)
      {
        tokens.emplace_back();
      }
      else
      {
        tokens.emplace_back(cur.substr(start, end - start + 1));
      }
    }
    return tokens;
  }

  static std::vector<std::string> splitWhitespace(const std::string &line)
  {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string tok;
    while (iss >> tok)
      tokens.push_back(tok);
    return tokens;
  }

  void parseAttributeFile()
  {
    std::ifstream in(attr_file_);
    if (!in)
    {
      throw std::runtime_error("Cannot open attribute file: " + attr_file_);
    }

    std::string line;
    if (!std::getline(in, line))
    {
      throw std::runtime_error("Empty attribute file: " + attr_file_);
    }

    std::string first_token;
    {
      std::istringstream iss(line);
      iss >> first_token;
    }

    bool first_is_int = isInteger(first_token);
    bool line_has_comma = (line.find(',') != std::string::npos);

    if (first_is_int && !line_has_comma)
    {
      if (!std::getline(in, line))
      {
        throw std::runtime_error("Unexpected end of file when reading header.");
      }
    }

    std::vector<std::string> header_tokens;
    if (line.find(',') != std::string::npos)
    {
      header_tokens = splitComma(line);
    }
    else
    {
      header_tokens = splitWhitespace(line);
    }

    if (header_tokens.empty())
    {
      throw std::runtime_error("Header line seems invalid.");
    }

    bool header_has_image_id = (header_tokens[0] == "image_id" ||
                                header_tokens[0] == "image_id " ||
                                header_tokens[0] == "image_id\r");

    if (header_has_image_id)
    {
      num_attrs_ = header_tokens.size() - 1;
      for (std::size_t i = 1; i < header_tokens.size(); ++i)
      {
        attr_index_[header_tokens[i]] = i - 1;
      }
    }
    else
    {
      num_attrs_ = header_tokens.size();
      for (std::size_t i = 0; i < header_tokens.size(); ++i)
      {
        attr_index_[header_tokens[i]] = i;
      }
    }

    all_meta_.clear();

    std::size_t line_no = 0;
    while (std::getline(in, line))
    {
      ++line_no;
      if (line.empty())
        continue;

      bool has_comma = (line.find(',') != std::string::npos);
      std::vector<std::string> tokens = has_comma ? splitComma(line)
                                                  : splitWhitespace(line);
      if (tokens.empty())
        continue;

      std::string filename;
      std::vector<std::string> attr_tokens;

      if (header_has_image_id)
      {
        if (tokens.size() < num_attrs_ + 1)
        {
          std::cerr << "Skipping malformed line " << line_no << " in " << attr_file_ << "\n";
          continue;
        }
        filename = tokens[0];
        attr_tokens.assign(tokens.begin() + 1,
                           tokens.begin() + 1 + (std::ptrdiff_t)num_attrs_);
      }
      else
      {
        if (tokens.size() < num_attrs_ + 1)
        {
          std::cerr << "Skipping malformed line " << line_no << " in " << attr_file_ << "\n";
          continue;
        }
        filename = tokens[0];
        attr_tokens.assign(tokens.begin() + 1,
                           tokens.begin() + 1 + (std::ptrdiff_t)num_attrs_);
      }

      Meta m;
      m.filename = filename;
      m.attrs.resize(num_attrs_);

      for (std::size_t j = 0; j < num_attrs_; ++j)
      {
        int v = std::stoi(attr_tokens[j]);
        if (v != 1 && v != -1)
        {
          std::cerr << "Warning: attribute value not in {-1,1} at line "
                    << line_no << " col " << j << ": " << v << "\n";
        }
        m.attrs[j] = static_cast<int8_t>((v >= 0) ? 1 : -1);
      }

      all_meta_.push_back(std::move(m));
    }

    if (all_meta_.empty())
    {
      throw std::runtime_error("No samples parsed from " + attr_file_);
    }
  }
};

int main()
{
  try
  {
    std::string images_root = "data/celeba/img_align_celeba";
    std::string attr_file = "data/celeba/list_attr_celeba.csv";
    std::string out_dir = "data/celeba_splits";

    CelebADatasetPreparer prep(images_root, attr_file);
    prep.loadAttributes();
    prep.createBalancedDataset(35000);
    prep.saveSplits(out_dir);

    std::cout << "Prepared " << prep.samples().size()
              << " balanced samples into " << out_dir << "\n";
    return 0;
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }
}
