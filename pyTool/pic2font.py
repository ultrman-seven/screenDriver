from PIL import Image
from PIL import ImageSequence
import numpy as np


def pic2bits(pic: Image, col=128, line=64):
    result = []
    for c in range(col):
        for page in range(int(line/8)):
            data = 0
            # range(page*8,page*8+8):
            for l in np.arange(page*8+8, page*8, -1):
                # data += int(pic.getpixel((l, c))/255)
                # data += int(pic.getpixel((c, l))/255)
                data <<= 1
                if pic.getpixel((c, int(l-1))) == 0:
                    data += 1
            result.append(data)
    return result


if __name__ == '__main__':
    img = Image.open('res/cir_32_32.png')
    # img = Image.open('res/logo.png')
    im = img.convert("1")
    im.show()
    d = pic2bits(im, 32, 32)
    print(d)
