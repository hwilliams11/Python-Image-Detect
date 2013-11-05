import numpy as np
import cv2
from copy import deepcopy
from matplotlib import pyplot as plt
    

HORIZONTAL = 0
VERTICAL = 1

class Div():
    divId = 0
    def __init__(self,row,col,width,height,children=None,divType=None):
        self.id = None
        self.row = row
        self.col = col
        self.width = width
        self.height = height
        self.divType = divType
        if children == None:
            self.children = []
        else:
            self.children=children

    @staticmethod
    def setId(div):
        div.id = Div.divId
        Div.divId = Div.divId + 1
        
    def __str__(self):
        return "(%d,%d) %d x %d " %(self.row,self.col,self.width,self.height)

    def __repr__(self):
        return self.__str__()
    
            

def createHTML(div,divType):

    if div == None:
        return
    if divType==HORIZONTAL:
        print "id: ",div.id,div.divType," float left"
    else:
        print "id: ",div.id,div.divType
    for child in div.children:
        createHTML(child,div.divType)
        
def createNewDiv(divList,horizontal):
    minRow = min(divList, key=lambda div: div.row).row
    maxRow = max(divList, key=lambda div: div.row+div.height)
    maxRow = maxRow.row+maxRow.height
    minCol = min(divList, key=lambda div: div.col).col
    maxCol = max(divList, key=lambda div: div.col+div.width)
    maxCol = maxCol.col+maxCol.width
    
    
    maxW = maxCol-minCol
    maxH = maxRow-minRow
    print "1",maxRow
    print "2",minCol
    print "3",maxW
    print "4",maxH

    if horizontal == True:
        divList.sort(key=lambda div:div.col)
        divType=HORIZONTAL
    else:
        divList.sort(key=lambda div:div.row)
        divType=VERTICAL
        
    div = Div(minRow,minCol,maxW,maxH,divList,divType)
    Div.setId(div)
    return div
    
def findHorizontalMatch(div1,divs):
    divList = [div1]
    
    for div2 in divs:
        if div1 != div2:
            if abs(div1.row-div2.row)<20 \
               and abs( div1.height - div2.height) < 50:
                divList.append(div2)
                divs.remove(div2)
    if len(divList)>1:
        divs.remove(div1)
        #    print divList
        div = createNewDiv(divList,True)
        divs.append(div)
    else:
        for div in divList:
            if div!=div1:
                divs.append(div)
    print divs
    return divs

def findVerticalMatch(div1,divs):
    divList = [div1]
    
    for div2 in divs:
        if div1 != div2:
            print "V1",div1.col,div2.col
            print "V2",div1.width,div2.width
            if abs(div1.col-div2.col)<20 \
               and abs( div1.width - div2.width) < 50:
                divList.append(div2)
                divs.remove(div2)
    if len(divList)>1:
        divs.remove(div1)
        #    print divList
        div = createNewDiv(divList,False)
        divs.append(div)
    else:
        for div in divList:
            if div!=div1:
                divs.append(div)
    print "Vert",divs
    return divs

def setupHierarchy(divs):
    print divs
    
    for div in divs:
        divs = findHorizontalMatch(div,divs)

    for div in divs:
        divs = findVerticalMatch(div,divs)

    return divs[0]

def similar(rect1,rect2):
    if abs(rect1.row-rect2.row) < 10 and \
         abs(rect1.col-rect2.col)< 10 and \
         abs(rect1.width-rect2.width)<10 and \
         abs(rect1.height-rect2.height)<10:
            return True
    else:
        return False
    
    
def removeDupRectangles(rects):
    newRectangles = deepcopy(rects)
    for rect in newRectangles:
        for otherRect in newRectangles:
            if rect != otherRect:
                if similar(rect,otherRect):
                    newRectangles.remove(otherRect)

    for rect in newRectangles:
        Div.setId(rect)
        
    return newRectangles
    
def setupRectangles(rects):
    print "Before"
    for rect in rects:
        print " (%d,%d) %d x %d " %(rect.row,rect.col,rect.width,rect.height)
    rects = removeDupRectangles(rects)
    print "After"
    for rect in rects:
        print " (%d,%d) %d x %d " %(rect.row,rect.col,rect.width,rect.height)
    mainDiv = setupHierarchy(rects)
    return mainDiv

    
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
        col,row,w,h = cv2.boundingRect(cnt)    
        #print "square",cnt
        if w*h > 10:
            print "square: (%d,%d) %d x %d " %(row,col,w,h)
            cv2.rectangle(img,(col,row),(col+w,row+h),(0,255,0),2)
            cv2.drawContours(img,[cnt],0,(0,0,255),-1)
            rects.append( Div(row,col,w,h) )
##    elif len(approx) == 9:
##        print "half-circle"
##        cv2.drawContours(img,[cnt],0,(255,255,0),-1)
##    elif len(approx) > 15:
##        print "circle"
##        cv2.drawContours(img,[cnt],0,(0,255,255),-1)

mainDiv = setupRectangles(rects)
createHTML(mainDiv,mainDiv.divType)

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
