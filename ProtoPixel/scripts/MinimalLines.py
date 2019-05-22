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
elapsedTimeLines = 0.0
startColorIndex = 1
endColorIndex = 2
scaleFactor = 10.0
lineScaleFactor = 10.0

rows = [ 0.0069, 0.055, 0.102, 0.151, 0.234926, 0.284, 0.331, 0.38, 0.479, 0.528, 0.575, 0.625, 0.707, 0.756, 0.803, 0.899, 0.948, 0.994]
cols = [ 0.026, 0.107, 0.186, 0.268089, 0.35, 0.428, 0.508, 0.591, 0.67, 0.751, 0.831, 0.912, 0.99]

content = Content("MinimalLines")
content.FBO_SIZE = (size,size) #optional: define size of FBO, default=(100,100)

content.add_parameter("color1", type="color", value=ofColor(255, 255, 255))
content.add_parameter("color2", type="color", value=ofColor(255, 255, 255))
content.add_parameter("color3", type="color", value=ofColor(255, 255, 255))
content.add_parameter("change_hue", value=True)
content.add_parameter("color_speed", min=0.00, max=1.0, value=0.1)
content.add_parameter("speed", min=0.0, max=1.0, value=0.1)
content.add_parameter("num_lines", min=0, max=10, value=1)
content.add_parameter("stage_mode", value=False)


class Line:

    size = 570
    height = 0
    width = 0
    animationType = 0
    animationTime = 1.0
    elapsedTime = 0.0
    xPos = 0
    yPos = 0
    endPos = 0
    startPos = 0

    def update(self):
        self.elapsedTime = self.elapsedTime + ofGetLastFrameTime()

        if self.animationType == 0:
            self.xPos = self.function(self.elapsedTime, self.startPos, self.endPos, self.animationTime)
          
        elif self.animationType == 1:
            self.xPos = self.function(self.elapsedTime, self.startPos, self.endPos, self.animationTime)
        
        elif self.animationType == 2:
            self.yPos = self.function(self.elapsedTime, self.startPos, self.endPos, self.animationTime)
        
        elif self.animationType == 3:
            self.yPos = self.function(self.elapsedTime, self.startPos, self.endPos, self.animationTime)

    def draw(self):
        ofDrawRectangle(self.xPos, self.yPos, self.width, self.height)

    def start(self, _animationType, pos):

        global cols, rows

        self.animationType = _animationType
        self.elapsedTime = 0.0

        if self.animationType == 0:
            self.width = self.size/2
            self.height = 10
            self.xPos = - self.width + cols[0]*size
            self.yPos = pos
            self.startPos = self.xPos
            self.endPos = cols[len(cols)-1]*size + self. width
          
        elif self.animationType == 1:
            self.width = self.size/2
            self.height = 10
            self.xPos = cols[len(cols)-1]*size
            self.yPos = pos
            self.startPos = self.xPos
            self.endPos = - self.width + cols[0]*size
        
        elif self.animationType == 2:
            self.height = self.size/2
            self.width = 10
            self.yPos = - self.height + rows[0]*size
            self.xPos = pos
            self.startPos = self.yPos
            self.endPos = rows[len(rows)-1]*size + self.height
        
        elif self.animationType == 3:
            self.height = self.size/2
            self.width = 10
            self.yPos = rows[len(rows)-1]*size + self.height
            self.xPos = pos
            self.startPos = self.yPos
            self.endPos = - self.height + rows[0]*size

        desc_str = "posx = %f posy = %f type = %i." % (self.xPos, self.yPos, self.animationType)

        #print desc_str

    def function(self, t, from_, to, duration):
        c = to - from_
        return c*(t/duration) + from_

    def setAnimationTime(self, animationTime):
        self.animationTime = animationTime
        #print self.animationTime



num_lines = 50
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

    global lines, lineScaleFactor

    time = ofMap(content['speed'], 0.0, 1.0, lineScaleFactor, lineScaleFactor/10.0)

    for line in lines:
       line.setAnimationTime(time)

    elapsedTimeLines = 0

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

@content.parameter_changed('stage_mode')
def parameter_changed(value):

    global size, cols, rows

    if value == True:
        rows = [ 0.899, 0.948, 0.994]
        cols = [ 0.268089, 0.35, 0.428, 0.508, 0.591, 0.67, 0.751]
    else:
        rows = [ 0.0069, 0.055, 0.102, 0.151, 0.234926, 0.284, 0.331, 0.38, 0.479, 0.528, 0.575, 0.625, 0.707, 0.756, 0.803, 0.899, 0.948, 0.994]
        cols = [ 0.026, 0.107, 0.186, 0.268089, 0.35, 0.428, 0.508, 0.591, 0.67, 0.751, 0.831, 0.912, 0.99]


       


def startLines():

    global size, cols, rows

    for i in range(0, content['num_lines']):
        type_ = randint(0, 1)
        if type_>1:
            n = randint(0, len(cols)-1)
            lines[i].start(type_, cols[n]*size -1)
        else: 
            n = randint(0, len(rows)-1)
            lines[i].start(type_, rows[n]*size - 1)

def setup():
    """
    This will be called at the beggining, you set your stuff here
    """
    ofSetBackgroundAuto(False)

    global color, lines,lineScaleFactor
    color = ofColor(content['color1'].r,content['color1'].g,content['color1'].b)

    time = ofMap(content['speed'], 0.0, 1.0, lineScaleFactor, lineScaleFactor/10.0)

    for i in range(0, 100):
        line = Line()
        line.setAnimationTime(time)
        lines.append(line)

    startLines()
        
       
def update():
    """
    For every frame, before drawing, we update stuff
    """

    global elapsedTime, color, startColorIndex, endColorIndex, lines, elapsedTimeLines, scaleFactor,lineScaleFactor

    for line in lines:
        line.update()

    elapsedTimeLines+=ofGetLastFrameTime()

    time = ofMap(content['speed'], 0.0, 1.0, lineScaleFactor, lineScaleFactor/10.0)

    if elapsedTimeLines>time:
        elapsedTimeLines = 0
        startLines()

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
