#!/usr/bin/env python

import os
from PIL import Image, ImageDraw
import math

#init_fill_color=(255,255,255)
init_fill_color=(0,0,0)

def to_gray(rgb_tuple):
    return (rgb_tuple[0] + rgb_tuple[1] + rgb_tuple[2]) / 3.0

def phase_i_init(im, fn_base):
    print(im.getpixel((0, 0)))
    threshold_lst = []
    for y in range(im.height):
        for x in range(im.width):
            pxl = im.getpixel((x, y))
            if pxl != init_fill_color:
                threshold_lst += to_gray(pxl),
    threshold_lst.sort()
    threshold_cut_percent = 0.1
    threshold_cut_hi_percent = 1 - 0.2
    threshold = threshold_lst[int(len(threshold_lst) * threshold_cut_percent)]
    threshold_hi = threshold_lst[int(len(threshold_lst) * threshold_cut_hi_percent)]
    print("threshold=%f" % threshold)
    im2 = im.copy()
    for y in range(im2.height):
        for x in range(im2.width):
            pxl = im2.getpixel((x, y))
            g = to_gray(pxl)
            if g < threshold or g > threshold_hi:
                im2.putpixel((x, y), init_fill_color)
    im2.save(fn_base + ".threshold_cut.bmp")

    # smooth image, remove isolated pixels

    # smooth_iter = 0
    # while True:
    #     smooth_iter += 1
    #     smooth_useful = False
    #     print("Smooth: iteration: %d" % smooth_iter)
    #     for y in range(1, im2.height -1):
    #         for x in range(1, im2.width-1):
    #             pxl = im3.getpixel((x, y))
    #             if pxl == init_fill_color:
    #                 continue
    #             nei = [
    #                 im3.getpixel((x-1, y-1)),
    #                 im3.getpixel((x, y-1)),
    #                 im3.getpixel((x+1, y-1)),
    #                 im3.getpixel((x-1, y)),
    #                 im3.getpixel((x+1, y)),
    #                 im3.getpixel((x-1, y+1)),
    #                 im3.getpixel((x, y+1)),
    #                 im3.getpixel((x+1, y+1)),
    #             ]
    #             isolate_cnt = 0
    #             for v in nei:
    #                 if v == init_fill_color:
    #                     isolate_cnt += 1
    #             if isolate_cnt >= 7:
    #                 im3.putpixel((x, y), init_fill_color)
    #                 smooth_useful = True
    #     if not smooth_useful:
    #         break
    #
    # im3.save(fn_base + ".threshold_cut_smooth.bmp")
    
    # tida paper like search, search radius = 2
    # for y in range(2, im2.height -2):
#         for x in range(2, im2.width-2):
#             pxl = to_gray(im2.getpixel((x, y)))
#             # top-bottom direction
#             p1 = to_gray(im2.getpixel((x, y - 2)))
#             p2 = to_gray(im2.getpixel((x, y - 1)))
#             p4 = to_gray(im2.getpixel((x, y + 1)))
#             p5 = to_gray(im2.getpixel((x, y +2)))
#             if (pxl == to_gray(init_fill_color) or
#                 p1 == to_gray(init_fill_color) or
#                 p2 == to_gray(init_fill_color) or
#                 p4 == to_gray(init_fill_color) or
#                 p5 == to_gray(init_fill_color) ):
#
#                 continue
#
#             is_top_dark_and_bottom_bright = (
#                 p1 > pxl and p2 > pxl and
#                 pxl < p4 and p4 <= p5
#             )
#             is_top_bright_and_bottom_dark = (
#                 p1 <= p2 and p2 < pxl and
#                 pxl > p4 and pxl > p5
#             )
#             if not is_top_dark_and_bottom_bright and not is_top_bright_and_bottom_dark:
#                 continue
#
#             # left-right direction
#             p1 = to_gray(im2.getpixel((x -2, y)))
#             p2 = to_gray(im2.getpixel((x-1, y)))
#             p4 = to_gray(im2.getpixel((x+1, y)))
#             p5 = to_gray(im2.getpixel((x+2, y)))
#             if (pxl == to_gray(init_fill_color) or
#                 p1 == to_gray(init_fill_color) or
#                 p2 == to_gray(init_fill_color) or
#                 p4 == to_gray(init_fill_color) or
#                 p5 == to_gray(init_fill_color) ):
#
#                 continue
#
#             is_top_dark_and_bottom_bright = (
#                 p1 > pxl and p2 > pxl and
#                 pxl < p4 and p4 <= p5
#             )
#             is_top_bright_and_bottom_dark = (
#                 p1 <= p2 and p2 < pxl and
#                 pxl > p4 and pxl > p5
#             )
#             if not is_top_dark_and_bottom_bright and not is_top_bright_and_bottom_dark:
#                 continue
#
#             # top-left - bottom-right direction
#             p1 = to_gray(im2.getpixel((x -2, y-2)))
#             p2 = to_gray(im2.getpixel((x-1, y-1)))
#             p4 = to_gray(im2.getpixel((x+1, y+1)))
#             p5 = to_gray(im2.getpixel((x+2, y+2)))
#             if (pxl == to_gray(init_fill_color) or
#                 p1 == to_gray(init_fill_color) or
#                 p2 == to_gray(init_fill_color) or
#                 p4 == to_gray(init_fill_color) or
#                 p5 == to_gray(init_fill_color) ):
#
#                 continue
#
#             is_top_dark_and_bottom_bright = (
#                 p1 > pxl and p2 > pxl and
#                 pxl < p4 and p4 <= p5
#             )
#             is_top_bright_and_bottom_dark = (
#                 p1 <= p2 and p2 < pxl and
#                 pxl > p4 and pxl > p5
#             )
#             if not is_top_dark_and_bottom_bright and not is_top_bright_and_bottom_dark:
#                 continue
#
#
#             # top-right - bottom-left direction
#             p1 = to_gray(im2.getpixel((x -2, y+2)))
#             p2 = to_gray(im2.getpixel((x-1, y+1)))
#             p4 = to_gray(im2.getpixel((x+1, y-1)))
#             p5 = to_gray(im2.getpixel((x+2, y-2)))
#             if (pxl == to_gray(init_fill_color) or
#                 p1 == to_gray(init_fill_color) or
#                 p2 == to_gray(init_fill_color) or
#                 p4 == to_gray(init_fill_color) or
#                 p5 == to_gray(init_fill_color) ):
#
#                 continue
#
#             is_top_dark_and_bottom_bright = (
#                 p1 > pxl and p2 > pxl and
#                 pxl < p4 and p4 <= p5
#             )
#             is_top_bright_and_bottom_dark = (
#                 p1 <= p2 and p2 < pxl and
#                 pxl > p4 and pxl > p5
#             )
#             if not is_top_dark_and_bottom_bright and not is_top_bright_and_bottom_dark:
#                 continue
#
#             im3.putpixel((x,y), (255, 0, 0))
#
#     # tida local minima, radius = 3
#     for y in range(3, im2.height -3):
#         for x in range(3, im2.width-3):
#             pxl = to_gray(im2.getpixel((x, y)))
#             # top-bottom direction
#             p1 = to_gray(im2.getpixel((x, y - 3)))
#             p2 = to_gray(im2.getpixel((x, y - 2)))
#             p3 = to_gray(im2.getpixel((x, y - 1)))
#             p4 = to_gray(im2.getpixel((x, y + 1)))
#             p5 = to_gray(im2.getpixel((x, y +2)))
#             p6 = to_gray(im2.getpixel((x, y +3)))
#             if (pxl == to_gray(init_fill_color) or
#                 p1 == to_gray(init_fill_color) or
#                 p2 == to_gray(init_fill_color) or
#                 p3 == to_gray(init_fill_color) or
#                 p6 == to_gray(init_fill_color) or
#                 p4 == to_gray(init_fill_color) or
#                 p5 == to_gray(init_fill_color) ):
#
#                 continue
#
#             is_top_dark_and_bottom_bright = (
#                 p1 > pxl and p2 > pxl and p3 > pxl and
#                 pxl < p4 and p4 <= p5 and p5 <= p6
#             )
#             is_top_bright_and_bottom_dark = (
#                 p4 > pxl and p5 > pxl and p6>pxl and
#                 pxl < p1 and p1 <= p2 and p2 <= p3
#             )
#             if is_top_dark_and_bottom_bright or is_top_bright_and_bottom_dark:
#                 im3.putpixel((x,y), (0, 0, 255))
#                 continue
#
#             # left-right direction
#             p1 = to_gray(im2.getpixel((x-3, y )))
#             p2 = to_gray(im2.getpixel((x-2, y )))
#             p3 = to_gray(im2.getpixel((x-1, y )))
#             p4 = to_gray(im2.getpixel((x+1, y )))
#             p5 = to_gray(im2.getpixel((x+2, y )))
#             p6 = to_gray(im2.getpixel((x+3, y )))
#             if (pxl == to_gray(init_fill_color) or
#                 p1 == to_gray(init_fill_color) or
#                 p2 == to_gray(init_fill_color) or
#                 p3 == to_gray(init_fill_color) or
#                 p6 == to_gray(init_fill_color) or
#                 p4 == to_gray(init_fill_color) or
#                 p5 == to_gray(init_fill_color) ):
#
#                 continue
#
#             is_top_dark_and_bottom_bright = (
#                 p1 > pxl and p2 > pxl and p3 > pxl and
#                 pxl < p4 and p4 <= p5 and p5 <= p6
#             )
#             is_top_bright_and_bottom_dark = (
#                 p4 > pxl and p5 > pxl and p6>pxl and
#                 pxl < p1 and p1 <= p2 and p2 <= p3
#             )
#             if is_top_dark_and_bottom_bright or is_top_bright_and_bottom_dark:
#                 im3.putpixel((x,y), (0, 0, 255))
#                 continue
#
#             # top-left - bottom-right direction
#             p1 = to_gray(im2.getpixel((x-3, y - 3)))
#             p2 = to_gray(im2.getpixel((x-2, y - 2)))
#             p3 = to_gray(im2.getpixel((x-1, y - 1)))
#             p4 = to_gray(im2.getpixel((x+1, y + 1)))
#             p5 = to_gray(im2.getpixel((x+2, y +2)))
#             p6 = to_gray(im2.getpixel((x+3, y +3)))
#             if (pxl == to_gray(init_fill_color) or
#                 p1 == to_gray(init_fill_color) or
#                 p2 == to_gray(init_fill_color) or
#                 p3 == to_gray(init_fill_color) or
#                 p6 == to_gray(init_fill_color) or
#                 p4 == to_gray(init_fill_color) or
#                 p5 == to_gray(init_fill_color) ):
#
#                 continue
#
#             is_top_dark_and_bottom_bright = (
#                 p1 > pxl and p2 > pxl and p3 > pxl and
#                 pxl < p4 and p4 <= p5 and p5 <= p6
#             )
#             is_top_bright_and_bottom_dark = (
#                 p4 > pxl and p5 > pxl and p6>pxl and
#                 pxl < p1 and p1 <= p2 and p2 <= p3
#             )
#             if is_top_dark_and_bottom_bright or is_top_bright_and_bottom_dark:
#                 im3.putpixel((x,y), (0, 0, 255))
#                 continue
#
#
#             # top-right - bottom-left direction
#             p1 = to_gray(im2.getpixel((x-3, y + 3)))
#             p2 = to_gray(im2.getpixel((x-2, y + 2)))
#             p3 = to_gray(im2.getpixel((x-1, y + 1)))
#             p4 = to_gray(im2.getpixel((x+1, y - 1)))
#             p5 = to_gray(im2.getpixel((x+2, y -2)))
#             p6 = to_gray(im2.getpixel((x+3, y -3)))
#             if (pxl == to_gray(init_fill_color) or
#                 p1 == to_gray(init_fill_color) or
#                 p2 == to_gray(init_fill_color) or
#                 p3 == to_gray(init_fill_color) or
#                 p6 == to_gray(init_fill_color) or
#                 p4 == to_gray(init_fill_color) or
#                 p5 == to_gray(init_fill_color) ):
#
#                 continue
#
#             is_top_dark_and_bottom_bright = (
#                 p1 > pxl and p2 > pxl and p3 > pxl and
#                 pxl < p4 and p4 <= p5 and p5 <= p6
#             )
#             is_top_bright_and_bottom_dark = (
#                 p4 > pxl and p5 > pxl and p6>pxl and
#                 pxl < p1 and p1 <= p2 and p2 <= p3
#             )
#             if is_top_dark_and_bottom_bright or is_top_bright_and_bottom_dark:
#                 im3.putpixel((x,y), (0, 0, 255))
#                 continue
#
#     im3.save(fn_base + ".tida_algorithm.bmp")
    
    # (r, x, y)
    trees = []
    finalized_trees = []
    
    for ys in range(0, im2.height-7, 7):
        for xs in range(0, im2.width-7, 7):
            cnt = 0
            for y in range(ys, ys + 7):
                for x in range(xs, xs + 7):
                    pxl = im2.getpixel((x, y))
                    if pxl != init_fill_color:
                        cnt += 1
            if cnt >= 7 * 7 * 0.5:
                trees += (4.9, x, y), # 4.9 = 3.5*1.414
            pass
    

    xxx = 0
    while True:
        xxx += 1
        if len(trees) == 0:
            break
        trees.sort()  # choose smallest tree
        t = trees[0]
        del trees[0]  # pop the tree

        t_radius = t[0]
        if t_radius > 20: # maybe add some randomness here
            # tree too big, do not merge any more
            finalized_trees += t,
            continue

        # try merge t with nearest other_t
        if len(trees) == 0:
            # no other trees
            finalized_trees += t,
            continue

        tr, tx, ty = t

        # (distance, other_tree_idx)
        merge_candidate = []

        for ot_idx in range(len(trees)):
            ot = trees[ot_idx]
            o_tr, o_tx, o_ty = ot
            dx = tx - o_tx
            dy = ty - o_ty
            d = math.sqrt(dx*dx + dy*dy)
            if d < tr + o_tr + 0.5:
                merge_candidate += (d, ot_idx),
        
        merge_candidate.sort()
        print("%d merge candidates, eg: " % len(merge_candidate), merge_candidate[0:3])
        
        if len(merge_candidate) == 0:
            # no other trees
            finalized_trees += t,
            continue
        else:
            # look at nearest (max 10) neighbors, maybe add a little bit of randomness here
            merge_candidates2 = [] # (cost, idx)
            for candi_i in range(0, min(10, len(merge_candidate))):
                merge_t_idx = merge_candidate[0][1]
                merge_t = trees[merge_t_idx]
                m_tr, m_tx, m_ty = merge_t
                merge_cost = should_i_merge_those_trees(im2, t, merge_t)
                merge_candidates2 += (merge_cost, merge_t_idx),

            merge_candidates2.sort()
            print("NEW MERGE CANDIDATES: ", merge_candidates2[0:4])
            
            candi = merge_candidates2[0]
            if candi[0] > 5.0:  # too much merge cost
                # TODO if cannot merge, mark t as finalized
                print("**** TOO MUCH MERGE COST")
                finalized_trees += t,
            else:
                # TODO if can merge, remove both t and merge_t from trees, add merged tree back to trees
                merge_t_idx = candi[1]
                merge_t = trees[merge_t_idx]
                merged_result = get_merged_tree(t, merge_t)
                merged_st = get_tree_stat(im2, merged_result)
                
                if merged_result[0] > 20 or merged_st["non_tree_ratio"] > 0.3:
                    # merged tree too big, or too many non tree area
                    finalized_trees += t,
                else:
                    del trees[merge_t_idx]
                    trees += merged_result,
        
        if xxx % 100 != 0:
            continue
        
        filter_finalized_trees(im2, finalized_trees)
        
        im3 = im2.copy()
        t_draw = ImageDraw.Draw(im3)
        
        #print(trees[0:3])
        print("%d leftover trees" % len(trees))
        for t in trees:
            tr, tx, ty = t
            t_draw.ellipse((int(tx-tr),int(ty-tr),int(tx+tr),int(ty+tr)), outline=(0,0,255))
            pass

        print("%d finalied trees" % len(finalized_trees))
        for t in finalized_trees:
            tr, tx, ty = t
            print("finalized tree:", t)
            t_draw.ellipse((int(tx-tr),int(ty-tr),int(tx+tr),int(ty+tr)), outline=(255,0,0))
            pass
        
        im3.save(fn_base + ".my_box_algo.bmp")
    
    filter_finalized_trees(im2, finalized_trees)
    
    im3 = im2.copy()
    t_draw = ImageDraw.Draw(im3)

    print("%d leftover trees" % len(trees))
    for t in trees:
        tr, tx, ty = t
        t_draw.ellipse((int(tx-tr),int(ty-tr),int(tx+tr),int(ty+tr)), outline=(0,0,255))
        pass

    print("%d finalied trees" % len(finalized_trees))
    for t in finalized_trees:
        tr, tx, ty = t
        print("finalized tree:", t)
        t_draw.ellipse((int(tx-tr),int(ty-tr),int(tx+tr),int(ty+tr)), outline=(255,0,0))
        pass
        
    im3.save(fn_base + ".my_box_algo.bmp")
    
    return im2


def filter_finalized_trees(im, finalized_trees):
    t_idx = 0
    while t_idx < len(finalized_trees):
        t = finalized_trees[t_idx]
        st = get_tree_stat(im, t)
        new_t = (t[0], st["tree_center_x"], st["tree_center_y"])
        finalized_trees[t_idx] = new_t
        t_idx += 1

    t_idx = 0
    while t_idx < len(finalized_trees):
        t = finalized_trees[t_idx]
        st = get_tree_stat(im, t)
        if st["non_tree_ratio"] > 0.4:
            del finalized_trees[t_idx]
            continue
        t_idx += 1


    finalized_trees.sort()

    t_idx = 0
    while t_idx < len(finalized_trees):
        should_remove = False
        small_t = finalized_trees[t_idx]
        for another_t_idx in range(len(finalized_trees)):
            if another_t_idx == t_idx:
                continue
            big_t = finalized_trees[another_t_idx]
            if big_t[0] < small_t[0]:
                continue
            dx = small_t[1] - big_t[1]
            dy = small_t[2] - big_t[2]
            d = math.sqrt(dx*dx + dy*dy)
            if small_t[0] + d < 1.7 * big_t[0]:
                should_remove = True
                break

        if should_remove:
            del finalized_trees[t_idx]
            merge_cost = should_i_merge_those_trees(im, small_t, big_t)
            if merge_cost <= 5.0:
                merged_t = get_merged_tree(small_t, big_t)
                st = get_tree_stat(im, merged_t)
                if merged_t[0] <= 20 and st["non_tree_ratio"] < 0.3:
                    # merged tree too big, or too many non tree area
                    del finalized_trees[another_t_idx - 1]
                    finalized_trees += merged_t,
        else:
            t_idx += 1


def get_tree_stat(im, t1):
    t1_colors = []
    t1_non_tree_cnt = 0
    t1_tree_cnt = 0
    t1_sum = 0.0
    tx_sum = 0
    ty_sum = 0
    t1r, t1x, t1y = t1
    for y in range(int(t1y-t1r), int(t1y+t1r) + 1):
        if y >= im.height or y < 0:
            continue
        for x in range(int(t1x-t1r), int(t1x+t1r) + 1):
            if x >= im.width or x < 0:
                continue
            dx = x - t1x
            dy = y - t1y
            d = math.sqrt(dx*dx + dy*dy)
            if d > t1r:
                continue
            pxl = im.getpixel((x, y))
            if pxl == init_fill_color:
                t1_non_tree_cnt+=1
                continue
            t1_tree_cnt+=1
            g = to_gray(pxl)
            t1_sum += g
            t1_colors += g,
            tx_sum += x
            ty_sum += y
    t1_avg = t1_sum * 1.0 / t1_tree_cnt
    st  ={}
    xs = 0.0
    for x in t1_colors:
        xs += (x - t1_avg) * (x - t1_avg)
    xs /= t1_tree_cnt
    st["tree_center_x"] = tx_sum * 1.0 / t1_tree_cnt
    st["tree_center_y"] = ty_sum * 1.0 / t1_tree_cnt
    st["avg"] = t1_avg
    st["variance"] = xs
    st["stddev"] = math.sqrt(xs)
    st["tree_cnt"] = t1_tree_cnt
    st["non_tree_cnt"] = t1_non_tree_cnt
    st["non_tree_ratio"] = t1_non_tree_cnt * 1.0 / (t1_tree_cnt + t1_non_tree_cnt)
    st["x"] = t1x
    st["y"] = t1y
    st["r"] = t1r
    return st

def get_merged_tree(t1, t2):
    t1r, t1x, t1y = t1
    t2r, t2x, t2y = t2
    
    assert t1r <= t2r
    
    dx = t1x-t2x
    dy = t1y-t2y
    
    d = math.sqrt(dx*dx +dy*dy)
    
    kx = (t2x - t1x) / d
    xl = t1x - kx * t1r
    xr = t2x + kx * t2r
    new_x = 0.5 * (xl + xr)
    
    ky = (t2y - t1y) / d
    yl = t1y - ky * t1r
    yr = t2y + ky * t2r
    new_y = 0.5 * (yl + yr)
    
    dx = xl-xr
    dy=yl-yr
    new_r = 0.5 * math.sqrt(dx*dx +dy*dy)
    
    return (new_r, new_x, new_y)

# smaller is better
def should_i_merge_those_trees(im, t1, t2):
    # ensure t1.r <= t2.r
    if t1[0] > t2[0]:
        return should_i_merge_those_trees(im, t2, t1)
    
    merged_t = get_merged_tree(t1, t2)
    
    t1st = get_tree_stat(im, t1)
    t2st = get_tree_stat(im, t2)
    tm_st = get_tree_stat(im, merged_t)

    print("")
    print("t1:", t1st)
    print("t2:", t2st)
    print("merged:", tm_st)
    
    cost1 = abs(t1st["avg"] - t2st["avg"]) / (1 + t1st["stddev"])
    cost2 = abs(t1st["avg"] - t2st["avg"]) / (1 + t2st["stddev"])
    non_tree_ratio_incr = 0.0

    max_non_t_ratio = max(t1st["non_tree_ratio"], t2st["non_tree_ratio"])
    if tm_st["non_tree_ratio"] > max_non_t_ratio:
        if max_non_t_ratio == 0:
            non_tree_ratio_incr = tm_st["non_tree_ratio"]
        else:
            non_tree_ratio_incr = (tm_st["non_tree_ratio"] - max_non_t_ratio) / max_non_t_ratio

    print("non tree ratio incr: %f" % non_tree_ratio_incr)
    print("cost1: %f" % cost1)
    print("cost2: %f" % cost2)
    
    return 10.0 * non_tree_ratio_incr + cost1 + cost2


def run_test(start_img_fn):
    print("RUN TEST: %s" % start_img_fn)
    fn_base = os.path.splitext(start_img_fn)[0]
    # pick out trees
    im = Image.open(start_img_fn)
    im_copy = im.copy()
    imdraw = ImageDraw.Draw(im_copy)
    imdraw.rectangle((0, 0, im_copy.width, im_copy.height), fill=init_fill_color)

    markers = []
    with open(fn_base + ".tree_marker.txt") as f:
        for l in f.readlines():
            l = l.strip()
            if l.startswith("TREE") or l.startswith("NOT_TREE"):
                markers += l,
            elif l.startswith("REMOVE"):
                s = l[l.index(" "):]
                remove_ok = False
                for idx in range(len(markers) - 1, -1, -1):
                    l2 = markers[idx]
                    s2 = l2[l2.index(" "):]
                    if s == s2:
                        del markers[idx]
                        remove_ok = True
                        break
                assert remove_ok

    # boxes = []
    draw = ImageDraw.Draw(im)
    for m in markers:
        sp = m.split()
        x = int(float(sp[1]) * im.width + 0.01)
        y = int(float(sp[2]) * im.height + 0.01)
        w = int(float(sp[3]) * im.width + 0.01)
        h = int(float(sp[4]) * im.height + 0.01)
        if sp[0] == "TREE":
            # boxes += ("TREE", x, y, w, h),
            crp = im.crop((x, y, x+w, y+h))
            im_copy.paste(crp, (x, y))
            pass
        elif sp[0] == "NOT_TREE":
            # boxes += ("NOT_TREE", x, y, w, h),
            draw.rectangle(x, y, x+w, y+h, fill=(255,255,255))

    im_copy.save(fn_base + ".tree_only.bmp")
    
    phase_i_init(im_copy, fn_base)


run_test("test-data/tree-valley-following-1.jpg")
run_test("test-data/tree-valley-following-2.jpg")
run_test("test-data/tree-valley-following-3.jpg")
