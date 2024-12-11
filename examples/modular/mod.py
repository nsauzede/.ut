#!/usr/bin/env python3

#
# UT - Simple and fast Unit Test framework
# Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
# SPDX-License-Identifier: GPL-3.0-or-later
#

# https://www.quantamagazine.org/behold-modular-forms-the-fifth-fundamental-operation-of-math-20230921/

import pygame
from modular import *

def draw_pix(d:dict):
    w,h=d["width"],d["height"]
    for j in range(h):
        for i in range(w):
            x,y=f4(pixtounit(w, h, (i,j)))
            angle=coord_angle((x,y))
            radius=coord_radius((x,y))
            mag=abs(math.log(radius))
            mag%=0.7
            mag*=2
            if mag>1:mag=1
            color = angle_color(angle, mag)
            d["screen"].set_at((i,j), color)
    pygame.display.flip()
    return 0
def main():
    pygame.init()
    d={ "width": 600, "height": 600, }
    d["screen"] = pygame.display.set_mode((d["width"], d["height"]))
    ret=draw_pix(d)
    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:  # Check for the window close event
                running = False
            if event.type == pygame.KEYDOWN:  # A key was pressed
                if event.key == pygame.K_ESCAPE:  # Check if the key is 'Q'
                    running = False
    pygame.quit()
    return ret
if __name__ == '__main__':
    main()
