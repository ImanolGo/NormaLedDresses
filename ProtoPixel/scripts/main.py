from openframeworks import *
from protopixel import Content
from tempfile import mkdtemp
import os.path
from random import randint
import imp


content = Content("NormaDress")

sparkle_file = content.add_asset("sparkle.py")
sparkles = imp.load_source('sparkles',sparkle_file)

rainbow_file = content.add_asset("rainbow.py")
rainbow = imp.load_source('rainbow',rainbow_file)

fade_file = content.add_asset("fade.py")
fade = imp.load_source('fade',fade_file)

waves_file = content.add_asset("waves.py")
waves = imp.load_source('waves',waves_file)

circles_file = content.add_asset("circles.py")
circles = imp.load_source('circles',circles_file)


print "NormaDress"


#a global variable
elapsedTime = 0.0
change_time = 60
size = 200
content.FBO_SIZE = (size,size) #optional: define size of FBO, default=(100,100)
targetAlpha = 1.0
currentAlpha = 0.0
currentColor = ofColor(255)
previousColor = ofColor(0)
shader = ofShader()
colorIndex = 0
modeIndex = 0


content.add_parameter("gamma", min=0.0, max=1.0, value=0.6)

content.add_parameter("enableSparkles", type="button")
content.add_parameter("enableRainbow", type="button")
content.add_parameter("enableFade", type="button")
content.add_parameter("enableWaves", type="button")
content.add_parameter("enableCircles", type="button")
content.add_parameter("Color", type="color", value=ofColor(255,255,255))


def setup():
    """
    This will be called at the beggining, you set your stuff here
    """

    global size, sparkles, rainbow, fade, waves, circles

    rainbow = rainbow.Rainbow(size,size)
    sparkles = sparkles.Sparkles(size,size)
    fade = fade.Fade(size,size)
    waves = waves.Waves(size,size)
    circles = circles.Circles(size,size)
    ofEnableAlphaBlending()

    setupShader()


def update():
    """
    For every frame, before drawing, we update stuff
    """

    global sparkles, rainbow

    updateTime()
    updateColor()

    sparkles.update()
    rainbow.update()
    fade.update()
    waves.update() 
    circles.update()

    #print "update"
        

def updateTime():
    global elapsedTime, change_time, colorIndex, modeIndex

    elapsedTime+=ofGetLastFrameTime()
    if elapsedTime>change_time:
        mode = randint(0,4)
        color = randint(5,11)
        print "changedModeColor"
        
        elapsedTime = 0
       


def updateColor():

    global currentAlpha, targetAlpha, previousColor, currentColor

    currentAlpha = currentAlpha + (targetAlpha - currentAlpha)*0.02
    
    if currentAlpha > 1.0:
        currentAlpha = 1.0

    newColor = previousColor.getLerped(currentColor, currentAlpha)
    setColors(newColor)
   # print currentAlpha
    #print newColor
    #print currentColor


def draw():
    """
    For every frame draw stuff. Do not forget to clear the frmebuffer!
    """
    global sparkles, shader

    ofClear(0)

    if shader.isLoaded():
        shader.begin()
        shader.setUniform1f('gamma', content["gamma"])
        
        ofClear(0)

        sparkles.draw()
        rainbow.draw()
        fade.draw()
        waves.draw()
        circles.draw()    

        shader.end()
  
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

def setColors(color):
    sparkles.setColor(color)
    fade.setColor(color)
    waves.setColor(color)
    circles.setColor(color)

def setAlphas(value):
    sparkles.setAlpha(value)
    waves.setAlpha(value)
    rainbow.setAlpha(value)
    fade.setAlpha(value)
    circles.setAlpha(value)

def setNewColor(color):

    global currentAlpha, targetAlpha, previousColor, currentColor

    previousColor = previousColor.getLerped(currentColor, currentAlpha)
    currentColor = color
    currentAlpha = 0
    targetAlpha = 1



@content.parameter_changed('Color')
def color_changed(value):
    setNewColor(value)
    #setColors(value)
  

@content.add_button('enableWaves')
def waves_button():
    setAlphas(0)
    waves.setAlpha(1.0)
    print "Waves pressed"

@content.add_button('enableSparkles')
def sparkles_button():
    setAlphas(0)
    sparkles.setAlpha(1.0)
    print "Sparkles: pressed"

@content.add_button('enableRainbow')
def rainbow_changed():
    setAlphas(0)
    rainbow.setAlpha(1.0)
    print "Rainbow pressed"

@content.add_button('enableFade')
def fade_changed():
    setAlphas(0)
    fade.setAlpha(1.0)
    print "Fade pressed "

@content.add_button('enableCircles')
def circles_changed():
    setAlphas(0)
    circles.setAlpha(1.0)
    print "Circles pressed"

def setupShader():

    global shader

    temp_dir = mkdtemp()
    frag_file = os.path.join(temp_dir,'s.frag')
    vert_file = os.path.join(temp_dir,'s.vert')
    shader_file_of = os.path.join(temp_dir,'s')

    vert_contents = """
    #version 150

    // these are for the programmable pipeline system
    uniform mat4 modelViewProjectionMatrix;

    in vec4 position;
    in vec2 texcoord;

    out vec2 texCoordVarying;

    void main()
    {
        texCoordVarying = texcoord;
        
        gl_Position = modelViewProjectionMatrix * position;
    }
    """

    frag_contents_prefix = """
    #version 150    // <-- version my machine suports

    uniform sampler2DRect texture0;
    uniform float gamma = 0.2;

    in vec2 texCoordVarying;

    out vec4 outputColor;
    """

    frag_contents = """
            
    void main(){


        vec2 pos = texCoordVarying;
            

        //Output of the shader
        outputColor = texture(texture0, pos);
        outputColor.rgb = pow(outputColor.rgb, vec3(1.0 / gamma));
        
    }

    """

    frag_contents_suffix = """
        
        
    """    

    with open(frag_file,'w') as f:
        f.write(frag_contents_prefix)
        f.write(frag_contents)
        f.write(frag_contents_suffix)
    with open(vert_file,'w') as f:
        f.write(vert_contents)
    shader.load(shader_file_of)






