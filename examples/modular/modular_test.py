from modular import *

def test_coord_angle():
    assert 0 == coord_angle((1,0))
    assert 45 == coord_angle((1,1))
    assert 90 == coord_angle((0,1))
    assert 135 == coord_angle((-1,1))
    assert 180 == coord_angle((-1,0))
    assert -135 == coord_angle((-1,-1))
    assert -90 == coord_angle((0,-1))
    assert -45 == coord_angle((1,-1))
def test_angle_color():
    assert angle_color(0) == (255, 0, 0)
    assert angle_color(45) == (255, 191, 0)
    assert angle_color(90) == (127, 255, 0)
    assert angle_color(135) == (0, 255, 63)
    assert angle_color(180) == (0, 255, 255)
    assert angle_color(-135) == (0, 63, 255)
    assert angle_color(-90) == (127, 0, 255)
    assert angle_color(-45) == (255, 0, 191)
def test_pixtounit():
    w,h=100,100
    assert (-1,-1)==pixtounit(w,h,(0,h-1))
    assert (1,-1)==pixtounit(w,h,(w-1,h-1))
    assert (1,1)==pixtounit(w,h,(w-1,0))
    assert (-1,1)==pixtounit(w,h,(0,0))
