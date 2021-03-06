//
// Created by antoinewdg on 11/15/16.
//

#ifndef PATCH_MATCH_MASKED_PATCHES_H
#define PATCH_MATCH_MASKED_PATCHES_H

#include <random>
#include <vector>

#include <opencv2/core/core.hpp>

#include "base_patch_server.h"

namespace pm {
    namespace opencv {

        using cv::Vec2i;
        using cv::Mat_;
        using cv::Rect;
        using cv::Size;

        using std::vector;

        template<class Predicate>
        class MaskedPatches : public BasePatchServer {
        public:

            typedef vector<Vec2i>::const_iterator const_iterator;
            typedef vector<Vec2i>::const_reverse_iterator const_reverse_iterator;

            MaskedPatches(const Mat_<bool> mask, int P) :
                    patches_mask(mask.size(), false),
                    BasePatchServer(mask.size(), P) {
                int p_2 = P / 2;
                Predicate predicate;
                for (int i = p_2; i < mask.rows - p_2; i++) {
                    for (int j = p_2; j < mask.cols - p_2; j++) {
                        if (predicate(mask, i, j, P)) {
                            patches.emplace_back(i, j);
                            patches_mask(i, j) = true;
                        }
                    }
                }

                index_dist = std::uniform_int_distribution<int>(0, patches.size() - 1);
            }

            inline const_iterator begin() const {
                return patches.begin();
            }

            inline const_iterator end() const {
                return patches.end();
            }

            inline const_reverse_iterator rbegin() const {
                return patches.rbegin();
            }

            inline const_reverse_iterator rend() const {
                return patches.rend();
            }

            inline bool contains_patch(const Vec2i &p) {
                return _is_patch_inside_boundaries(p) &&
                       patches_mask(p);
            }

            template<class RandomEngine>
            inline Vec2i pick_random(RandomEngine &generator) {
                return patches[index_dist(generator)];
            }

        private:
            Mat_<bool> patches_mask;
            vector<Vec2i> patches;
            std::uniform_int_distribution<int> index_dist;
        };

        struct AtLeastOneInPatch {
            bool operator()(const Mat_<bool> &mask, int i, int j, int P) {
                int p_2 = P / 2;
                int n = cv::countNonZero(mask(Rect(j - p_2, i - p_2, P, P)));
                return n > 0;
            }
        };

        struct AllInPatch {
            bool operator()(const Mat_<bool> &mask, int i, int j, int P) {
                int p_2 = P / 2;
                int n = cv::countNonZero(mask(Rect(j - p_2, i - p_2, P, P)));
                return n == P * P;
            }
        };

        typedef MaskedPatches<AtLeastOneInPatch> PartiallyMaskedPatches;
        typedef MaskedPatches<AllInPatch> TotallyMaskedPatches;


    }
}

#endif //PATCH_MATCH_MASKED_PATCHES_H
