from openframeworks import *
from protopixel import Content
from random import randint
import math

print "MinimalLines"

#a global variable
size = 570
width = size
height =  size
color = ofColor(255)
elapsedTime = 0.0
startColorIndex = 1
endColorIndex = 2
scaleFactor = 10.0
lineScaleFactor = 10.0

content = Content("MinimalLines")
content.FBO_SIZE = (size,size) #optional: define size of FBO, default=(100,100)

content.add_parameter("color1", type="color", value=ofColor(255, 255, 255))
content.add_parameter("color2", type="color", value=ofColor(255, 255, 255))
content.add_parameter("color3", type="color", value=ofColor(255, 255, 255))
content.add_parameter("change_hue", value=True)
content.add_parameter("color_speed", min=0.00, max=1.0, value=0.1)
content.add_parameter("speed", min=0.0, max=1.0, value=0.1)
content.add_parameter("num_lines", min=0, max=10, value=1)
content.add_parameter("num_rows", min=0, max=50, value=10)


class Line:

    def __init__(self, width, height, num_lines, start, speed):

        self.width = width
        self.height = height
        self.num_lines = num_lines
        self.start = start
        self.speed = speed + ofRandomf()*(speed*0.2)
        self.__initializeAttributes()
    
    def __initializeAttributes(self):
        self.xPos = 0
        self.yPos = 0
        num = ofRandomf()
        #print num
        if(num>0):
            self.directionX = 1.0
        else:
            self.directionX = -1.0

        print self.directionX

        num = ofRandomf()
        if(num>0):
            self.directionY = 1
        else:
            self.directionY = -1

        self.rows = []
        self.h = self.height/self.num_lines
        self.w = 10

        for i in range(self.num_lines):
            y = i*self.h
            self.rows.append(y)

        self.index = int(self.start*(self.num_lines-1))
        self.yPos = self.rows[self.index]
        self.xPos = ofRandom(0.0,self.width)



    def update(self):

        self.xPos +=(self.directionX*self.speed)
        self.yPos = self.rows[self.index]

        if self.xPos>self.width or self.xPos < 0-self.w :
            self.directionX = -self.directionX
            self.index = self.index + self.directionY
            if self.index >= self.num_lines or self.index < 0:
                self.directionY = -self.directionY
                self.index = self.index + 2*self.directionY


    def draw(self):
        ofDrawRectangle(self.xPos, self.yPos, self.w, self.h)

    def setSpeed(self, value):
        self.speed = value + ofRandomf()*(value*0.2)

num_lines = 2
num_rows = 2

lines = []


@content.parameter_changed('change_hue')
def parameter_changed(value):
    """
    This function is called every time a a_integer is changed.
    We get the new value as an argument
    """
    global color

    if value == False:
        color.r = content['color1'].r
        color.g = content['color1'].g
        color.b = content['color1'].b
        elapsedTime = 0
        startColorIndex = 1
        endColorIndex = 2

@content.parameter_changed('speed')
def parameter_changed(value):
    """
    This function is called every time a a_integer is changed.
    We get the new value as an argument
    """

    global lines

    for line in lines:
       line.setSpeed(content['speed'])

@content.parameter_changed('num_lines')
def parameter_changed(value):
    """
    This function is called every time a a_integer is changed.
    We get the new value as an argument
    """
    
    global lines 

    diff = content['num_lines'] - len(lines)
    print diff
    if diff>0:
        for i in range(0,diff):
            l = Line(size,size,content['num_rows'], ofRandomuf(), content['speed'] )
            lines.append(l)
    elif diff<0:
        first = len(lines) - abs(diff)
        last = len(lines)
        del lines[first:last]


@content.parameter_changed('num_rows')
def parameter_changed(value):
    """
    This function is called every time a a_integer is changed.
    We get the new value as an argument
    """
    setupLines()

@content.parameter_changed('color1')
def parameter_changed(value):
    """
    This function is called every time a a_integer is changed.
    We get the new value as an argument
    """
    global color

    print value
    if content['change_hue'] == False:
        color.r = content['color1'].r
        color.g = content['color1'].g
        color.b = content['color1'].b


@content.OSC('/norma/num_lines')
def oscNumLines(i):
    print "/norma/num_lines" + str(i) 

    global lines 

    content['num_lines'] = i

    diff = content['num_lines'] - len(lines)
    print diff
    if diff>0:
        for i in range(0,diff):
            l = Line(size,size,content['num_rows'], ofRandomuf(), content['speed'] )
            lines.append(l)
    elif diff<0:
        first = len(lines) - abs(diff)
        last = len(lines)
        del lines[first:last]

@content.OSC('/norma/num_rows')
def oscNumRows(i):
    print "/norma/num_rows" + str(i) 
    content['num_rows'] = i
    setupLines()

@content.OSC('/norma/speed')
def oscNumRows(x):
    global lines
    print "/norma/speed" + str(x) 
    content['speed'] = x

    for line in lines:
       line.setSpeed(content['speed'])


def setupLines():

    global lines 

    lines = []
    for i in range(0, content['num_lines']):
        l = Line(size,size,content['num_rows'], ofRandomuf(), content['speed'] )
        lines.append(l)

def setup():
    """
    This will be called at the beggining, you set your stuff here
    """
    ofSetBackgroundAuto(False)

    global color, lines,lineScaleFactor
    color = ofColor(content['color1'].r,content['color1'].g,content['color1'].b)
    time = ofMap(content['speed'], 0.0, 1.0, lineScaleFactor, lineScaleFactor/10.0)

    setupLines()
        
       
def update():
    """
    For every frame, before drawing, we update stuff
    """

    global elapsedTime, color, startColorIndex, endColorIndex, lines, scaleFactor,lineScaleFactor

    for line in lines:
        line.update()

    time = ofMap(content['speed'], 0.0, 1.0, lineScaleFactor, lineScaleFactor/10.0)

    if content['change_hue'] == False:
        return

    
    elapsedTime+=ofGetLastFrameTime()
    
    time = ofMap(content['color_speed'], 0,1, scaleFactor, scaleFactor/20.0)

    if elapsedTime>time:
        elapsedTime = 0
        startColorIndex = endColorIndex
        endColorIndex = (endColorIndex+1)%3 + 1

    amt = elapsedTime/(time)
    startColorStr = 'color' + str(startColorIndex)
    endColorStr = 'color' + str(endColorIndex)
    color.r = int(ofLerp(content[startColorStr].r, content[endColorStr].r, amt))
    color.g = int(ofLerp(content[startColorStr].g, content[endColorStr].g, amt))
    color.b = int(ofLerp(content[startColorStr].b, content[endColorStr].b, amt))
    

  
def draw():
    """
    For every frame draw stuff. Do not forget to clear the frmebuffer!
    """
    global color, xPos, yPos, width, height, size

    ofSetColor(0,0,0,6)
    ofDrawRectangle(0,0, float(size),float(size))
    ofFill()
    #ofClear(0)
    ofSetColor(color)

    # for row in rows:
    #     ofDrawRectangle(0, row*size - 2, width, 4)

    # for col in cols:
    #     ofDrawRectangle(col*size - 2, 0, 4, height)

    for line in lines:
        line.draw()

    #ofDrawRectangle(xPos, yPos, width, height)
    #ofDrawRectangle(0, 0, 170, 170)
   

def exit():
    """
    Before removing the script, in case you have pending business.
    """
    pass


def on_enable():
    """
    This function is called when this content just got enabled.
    """
    pass


def on_disable():
    """
    This function is called when this content just got disabled.
    `update` and `draw` functions are not called while this content is disabled.
    """
    pass
