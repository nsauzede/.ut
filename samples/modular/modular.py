#
# UT - Simple and fast Unit Test framework
# Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
# SPDX-License-Identifier: GPL-3.0-or-later
#

# https://www.quantamagazine.org/behold-modular-forms-the-fifth-fundamental-operation-of-math-20230921/

import math
def angle_color(angle:int, mag=1.0)->tuple:
    # Normalize the angle to [0, 360)
    hue = (angle % 360 + 360) % 360
    # HSV to RGB conversion
    C = 1*mag  # Full saturation and brightness
    X = C * (1 - abs((hue / 60) % 2 - 1))
    m = 0  # We skip brightness adjustment for simplicity
    if 0 <= hue < 60:
        r, g, b = C, X, 0
    elif 60 <= hue < 120:
        r, g, b = X, C, 0
    elif 120 <= hue < 180:
        r, g, b = 0, C, X
    elif 180 <= hue < 240:
        r, g, b = 0, X, C
    elif 240 <= hue < 300:
        r, g, b = X, 0, C
    else:  # 300 <= hue < 360
        r, g, b = C, 0, X
    # Convert to 0–255 scale
    return (int((r + m) * 255), int((g + m) * 255), int((b + m) * 255))
def coord_angle(coord:tuple)->int:
    angle=math.atan2(coord[1],coord[0])
    return math.degrees(angle)
def coord_radius(coord:tuple)->int:
    x,y=coord
    return math.sqrt(x*x+y*y)
def pixtounit(w:int, h:int, coord:tuple)->tuple:
    x=2.*coord[0]/(w-1)-1
    y=(h-1-2*coord[1])/(h-1)
    return (x,y)
def f2(z:tuple)->tuple:
    a,b=z
    return a*a-b*b,2*a*b
def f4(z:tuple,m=0.,n=1.)->tuple:
    z_=complex(z[0],z[1])
    fn=lambda c,m,n:1/((m+n*c)**4)
    z__=complex(0,0)
    z__+=fn(z_,0,1)
    z__+=fn(z_,0.5,1)
    z__+=fn(z_,0.5,-1)
    z__+=0.5
    return (z__.real, z__.imag)
def ftrig(z:tuple)->tuple:
    a,b=z
    return math.cos(a),math.sin(b)
def f1(z:tuple)->tuple:
    return z
