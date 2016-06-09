#!/usr/bin/env python

import json

def dump_geojson(fname):
    with open(fname) as f:
        d = json.loads(f.read())
    print(len(d))
    print(d.keys())
#    print(json.dumps(d, sort_keys=True, indent=4))

    tree_points = []

    x1 = -74.0112179826541
    x2 = -73.9932543613726
    y1 = 40.7275462944844
    y2 = 40.7412107739821

    for t in d["features"]:
        if not t["geometry"]:
            continue
        c = t["geometry"]["coordinates"]
        if c[0] < -74.0112179826541 or c[0] > -73.9932543613726 or c[1] < 40.7275462944844 or c[1] > 40.7412107739821:
            continue
        print(json.dumps(t, sort_keys=True, indent=4))
        print(t["geometry"]["coordinates"])
        tp = ( (c[0] - x1) / (x2 - x1), (c[1] - y1) / (y2 - y1))
        tree_points += tp,
        print(tp)
        print("\n")

    print(tree_points)

    in_img = "/Users/santa/Dropbox/wsq_park/18TWL835090_201203_0x6000m_4B_1.jpg"
    out_img = "/Users/santa/Dropbox/wsq_park/__OUT__.jpg"
    from PIL import Image, ImageDraw
    in_im = Image.open(in_img)
    print(in_im.width, in_im.height)
    draw = ImageDraw.Draw(in_im)
    for p in tree_points:
        x = p[0] * in_im.width
        y = (1 - p[1]) * in_im.height
        r = 1
        draw.ellipse((x-r,y-r,x+r,y+r), fill=(255,0,0,0))
    in_im.show()
    in_im.save(out_img)

if __name__ == "__main__":
    import sys
    dump_geojson(sys.argv[1])
