package com.classifier.utils;

import java.util.HashMap;
import java.util.Map;
import java.util.ArrayList;
import java.util.List;

// Generic class for caching results
public class Cache<K, V> {
    private Map<K, V> cache;
    private int maxSize;

    public Cache(int maxSize) {
        this.cache = new HashMap<>();
        this.maxSize = maxSize;
    }

    public void put(K key, V value) {
        if (cache.size() >= maxSize) {
            // Remove oldest entry (simple strategy)
            K firstKey = cache.keySet().iterator().next();
            cache.remove(firstKey);
        }
        cache.put(key, value);
    }

    public V get(K key) {
        return cache.get(key);
    }

    public boolean contains(K key) {
        return cache.containsKey(key);
    }

    public void clear() {
        cache.clear();
    }

    public int size() {
        return cache.size();
    }

    public List<V> getAllValues() {
        return new ArrayList<>(cache.values());
    }
}
