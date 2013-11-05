import cv2
import numpy as np
from matplotlib import pyplot as plt


def adaptive():
    img = cv2.imread('shapes.png',0)
    img = cv2.medianBlur(img,5)
     
    ret,th1 = cv2.threshold(img,127,255,cv2.THRESH_BINARY)
    th2 = cv2.adaptiveThreshold(img,255,cv2.ADAPTIVE_THRESH_MEAN_C,\
                cv2.THRESH_BINARY,11,2)
    th3 = cv2.adaptiveThreshold(img,255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C,\
                cv2.THRESH_BINARY,11,2)
     
    plt.subplot(2,2,1),plt.imshow(img,'gray')
    plt.title('input image')
    plt.subplot(2,2,2),plt.imshow(th1,'gray')
    plt.title('Global Thresholding')
    plt.subplot(2,2,3),plt.imshow(th2,'gray')
    plt.title('Adaptive Mean Thresholding')
    plt.subplot(2,2,4),plt.imshow(th3,'gray')
    plt.title('Adaptive Gaussian Thresholding')
     
    plt.show()

def thresholding():
     
    img = cv2.imread('shapes_gray.png',0)
    ret,thresh1 = cv2.threshold(img,127,255,cv2.THRESH_BINARY)
    ret,thresh2 = cv2.threshold(img,127,255,cv2.THRESH_BINARY_INV)
    ret,thresh3 = cv2.threshold(img,127,255,cv2.THRESH_TRUNC)
    ret,thresh4 = cv2.threshold(img,127,255,cv2.THRESH_TOZERO)
    ret,thresh5 = cv2.threshold(img,127,255,cv2.THRESH_TOZERO_INV)
     
    thresh = ['img','thresh1','thresh2','thresh3','thresh4','thresh5']
     
    for i in xrange(6):
        plt.subplot(2,3,i+1),plt.imshow(eval(thresh[i]),'gray')
        plt.title(thresh[i])
     
    plt.show()

thresholding()
