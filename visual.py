# -*- coding: utf-8 -*-
# @Author: TimeEternal
# @Date:   2019-06-17 18:36:26
# @Last Modified by:   TimeEternal
# @Last Modified time: 2019-06-17 18:39:49

fx = 720.094219155800
fy = 717.971575468539
u0 = 751.717811957061
v0 = 364.267253691764
width = 1280
height = 800
Near = float(input("distance： "))
left=-u0*Near/fx
right=(width-u0)*Near/fx
bottom=(v0-height)*Near/fy
top=v0*Near/fy
print(left, right, bottom, top)