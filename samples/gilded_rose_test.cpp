/*
* UT - Simple and fast Unit Test framework
* Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#include <string>
#include <sstream>
#include <vector>
struct Item {
    // std::string name; int sell_in; int quality;
    const char* name; int sell_in; int quality;
    std::string to_str() const {
        std::ostringstream oss;
        oss << "{sell_in=" << sell_in << ", quality=" << quality << "}";
        return oss.str();
    }
};
struct GildedRose {
    std::vector<Item>& items;
    GildedRose(std::vector<Item>& items):items(items){}
    void update_quality_brie(Item& item) {
        item.sell_in--;
        item.quality++;
        if (item.sell_in < 0) {
            item.quality++;
        }
        if (item.quality > 50) {
            item.quality = 50;
        }
    }
    void update_quality_sulfuras(Item& item) {
    }
    void update_quality_backstage(Item& item) {
        item.sell_in--;
        item.quality++;
        if (item.sell_in < 10) {
            item.quality++;
        }
        if (item.sell_in < 5) {
            item.quality++;
        }
        if (item.sell_in <= 0) {
            item.quality = 0;
        }
    }
    void update_quality_normal(Item& item) {
        item.sell_in--;
        item.quality--;
        if (item.sell_in < 0) {
            item.quality--;
        }
        if (item.quality < 0) {
            item.quality = 0;
        }
    }
    void update_quality() {
        for (auto& item: items) {
            // if (item.name == "Backstage passes to a TAFKAL80ETC concert") {
            if (item.name == std::string("Backstage passes to a TAFKAL80ETC concert")) {
            // if (!strcmp(item.name, "Backstage passes to a TAFKAL80ETC concert")) {
                update_quality_backstage(item);
            } else if (item.name == std::string("Sulfuras, Hand of Ragnaros")) {
                update_quality_sulfuras(item);
            } else if (item.name == std::string("Aged Brie")) {
                update_quality_brie(item);
            } else {
                update_quality_normal(item);
            }
        }
    }
};
/******************************************************************************/
#include "ut/ut.h"
#include <iostream>
struct CaseOfTest {
    int days; int quality; int exp_quality; int exp_days;
    std::string to_str() const {
        std::ostringstream oss;
        oss << "{days=" << days << ", quality=" << quality << ", exp_quality=" << exp_quality << ", exp_days=" << exp_days << "}";
        return oss.str();
    }
};
TESTCASE(TestGildedRose)
    void composed_assert(const char *item, std::vector<CaseOfTest> test_cases) {
        for (const auto& tc: test_cases) {
            std::cout << "item=" << item << " tc=" << tc.to_str() << "\n";
            std::vector<Item> items = {{item, tc.days, tc.quality}};
            auto gilded_rose = GildedRose(items);
            gilded_rose.update_quality();
            ASSERT_EQ(items[0].name, item);
            ASSERT_EQ(items[0].quality, tc.exp_quality);
            ASSERT_EQ(items[0].sell_in, tc.exp_days);
        }
    }
    TESTMETHOD(test_backstage_passes_has_special_rules) {
        std::vector<CaseOfTest> tc = {
            {.days=20, .quality=43, .exp_quality=44, .exp_days=19},
            {.days=9, .quality=43, .exp_quality=45, .exp_days=8},
            {.days=4, .quality=43, .exp_quality=46, .exp_days=3},
            {.days=0, .quality=43, .exp_quality=0, .exp_days=-1},
        };
        composed_assert("Backstage passes to a TAFKAL80ETC concert", tc);
    }
    TESTMETHOD(test_sulfuras_never_changes) {
        std::vector<CaseOfTest> tc = {
            {.days=20, .quality=43, .exp_quality=43, .exp_days=20},
            {.days=9, .quality=43, .exp_quality=43, .exp_days=9},
            {.days=0, .quality=43, .exp_quality=43, .exp_days=0},
            {.days=0, .quality=50, .exp_quality=50, .exp_days=0},
        };
        composed_assert("Sulfuras, Hand of Ragnaros", tc);
    }
    TESTMETHOD(test_aged_brie_increases_quality) {
        std::vector<CaseOfTest> tc = {
            {.days=20, .quality=43, .exp_quality=44, .exp_days=19},
            {.days=9, .quality=43, .exp_quality=44, .exp_days=8},
            {.days=0, .quality=43, .exp_quality=45, .exp_days=-1},
            {.days=0, .quality=50, .exp_quality=50, .exp_days=-1},
        };
        composed_assert("Aged Brie", tc);
    }
    TESTMETHOD(test_normal_decreases_quality) {
        std::vector<CaseOfTest> tc = {
            {.days=20, .quality=43, .exp_quality=42, .exp_days=19},
            {.days=9, .quality=43, .exp_quality=42, .exp_days=8},
            {.days=0, .quality=43, .exp_quality=41, .exp_days=-1},
            {.days=0, .quality=50, .exp_quality=48, .exp_days=-1},
            {.days=2, .quality=0, .exp_quality=0, .exp_days=1},
        };
        composed_assert("normal item", tc);
    }
