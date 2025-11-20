## CelebA Receding Hairline Split

This module builds a **balanced binary classification dataset** from CelebA for **receding hairline vs non–receding hairline**.

### What it does

- Uses CelebA attributes file (`list_attr_celeba.txt` or `list_attr_celeba.csv`).
- Target label: **Receding_Hairline**

  - `label = 1` → `Receding_Hairline = 1`
  - `label = 0` → `Receding_Hairline = -1`

- Uses **all genders** (no Male filter anymore).
- Balances the **negative class** across:

  - `Young` vs not `Young`
  - `Attractive` vs not `Attractive`

- Caps total dataset size at **≤ 35,000** samples.
- Outputs 3 split files with lines:

  ```text
  <image_path> <label>
  ```

### Files

- `download_celeba.sh`
  Downloads and unpacks CelebA into `data/celeba/` (images + attributes).

- `main.cpp`
  Contains the `CelebADatasetPreparer` class and `main()` that:

  - Reads CelebA attributes.
  - Creates the balanced receding-hairline dataset.
  - Writes:

    - `data/celeba_splits/train.txt`
    - `data/celeba_splits/val.txt`
    - `data/celeba_splits/test.txt`

### Usage

1. Download CelebA:

   ```bash
   chmod +x download_celeba.sh
   ./download_celeba.sh
   ```

2. Build and run:

   ```bash
   g++ -std=c++17 -O2 main.cpp -o prepare_celeba
   ./prepare_celeba
   ```

3. Use the generated `train/val/test.txt` file lists in your own data loader.
