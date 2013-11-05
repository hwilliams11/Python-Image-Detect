import numpy as np
import cv2
from matplotlib import pyplot as plt
from copy import deepcopy

img = cv2.imread('shapes_gray.png',0)

ret,thresh = cv2.threshold(img,127,255,cv2.THRESH_BINARY)
#thresh, im_bw = cv2.threshold(im_gray, 128, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)
thresh2 = deepcopy(thresh)

contours,h = cv2.findContours(thresh,1,2)

for cnt in contours:
    approx = cv2.approxPolyDP(cnt,0.01*cv2.arcLength(cnt,True),True)
    print len(approx)
    if len(approx)==5:
        print "pentagon"
        cv2.drawContours(thresh,[cnt],0,255,-1)
    elif len(approx)==3:
        print "triangle"
        cv2.drawContours(thresh,[cnt],0,(0,255,0),-1)
    elif len(approx)==4:
        print "square",cnt
        cv2.drawContours(thresh,[cnt],0,(0,0,255),-1)
    elif len(approx) == 9:
        print "half-circle"
        cv2.drawContours(thresh,[cnt],0,(255,255,0),-1)
    elif len(approx) > 15:
        print "circle"
        cv2.drawContours(thresh,[cnt],0,(0,255,255),-1)

plt.subplot(2,2,1)
plt.imshow(img,'gray')
plt.subplot(2,2,2)        
plt.imshow(thresh2,'gray')
plt.subplot(2,2,3)
plt.imshow(thresh,'gray')
plt.show()

