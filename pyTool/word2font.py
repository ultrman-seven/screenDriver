from PIL import Image, ImageFont, ImageDraw
import os
from PIL import Image, ImageFont, ImageDraw
import sys

# u4e00-u9fa5 (中文)
# (0x3400, 0x4DB5)
font = ImageFont.truetype(os.path.join("fonts", "Deng.ttf"), 16)


img = Image.new("RGB", size=(16, 16), color="white")


def getZH_char_code(text) -> list:
    im = img.convert("1")
    dr = ImageDraw.Draw(im)
    dr.text((0, 0), text, font=font, fill="#000000")
    # im.show()
    result = []
    for col in range(16):
        for page in range(2):
            data = 0
            for line in range(8):
                data >>= 1
                # print(im.getpixel((col,page*8+line)))
                if(im.getpixel((col, page*8+line)) == 0):
                    data += 0x80
            result.append(data)
    return result

if __name__ == '__main__':
    s = sys.argv[1]
    # c=getZH_char_code("你")
    for c in s:
        print(c)
        d=getZH_char_code(c)
        print(d)

