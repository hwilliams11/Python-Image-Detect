import numpy as np
import cv2
from copy import deepcopy
from matplotlib import pyplot as plt

def setupRectangles(rects):
    for rect in rects:
        print "(%d,%d) %d x %d " %(rect['x'],rect['y'],rect['width'],rect['height'])

img = cv2.imread('rects2.png')
gray = cv2.imread('rects2.png',0)
orig = deepcopy(img)
ret,thresh = cv2.threshold(gray,127,255,1)
contours,h = cv2.findContours(thresh,1,2)
rects = []

for cnt in contours:
    
    approx = cv2.approxPolyDP(cnt,0.01*cv2.arcLength(cnt,True),True)
    #print len(approx)
    if len(approx)==5:
        pass
        #print "pentagon"
        #cv2.drawContours(img,[cnt],0,255,-1)
##    elif len(approx)==3:
##        print "triangle"
##        cv2.drawContours(img,[cnt],0,(0,255,0),-1)
    elif len(approx)==4:
        x,y,w,h = cv2.boundingRect(cnt)    
        #print "square",cnt
        if w*h > 10:
            print "square: (%d,%d) %d x %d " %(x,y,w,h)
            cv2.rectangle(img,(x,y),(x+w,y+h),(0,255,0),2)
            cv2.drawContours(img,[cnt],0,(0,0,255),-1)
            rects.append({'x':x,'y':y,'width':w,'height':h})
##    elif len(approx) == 9:
##        print "half-circle"
##        cv2.drawContours(img,[cnt],0,(255,255,0),-1)
##    elif len(approx) > 15:
##        print "circle"
##        cv2.drawContours(img,[cnt],0,(0,255,255),-1)

setupRectangles(rects)

plt.subplot(2,2,1)
plt.imshow(orig,'gray')
plt.subplot(2,2,2)        
plt.imshow(gray,'gray')
plt.subplot(2,2,3)
plt.imshow(thresh,'gray')
plt.subplot(2,2,4)
plt.imshow(img,'gray')
plt.show()

#cv2.imshow('img',img)
#cv2.waitKey(0)
cv2.destroyAllWindows()
