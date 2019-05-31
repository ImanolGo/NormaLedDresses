from openframeworks import *
from protopixel import Content
from tempfile import mkdtemp
import os.path
from random import randint
import imp

from OSC import OSCClient, OSCMessage

content = Content("Norma")

video_file = content.add_asset("video_manager.py")
videos = imp.load_source('videos',video_file)

content.add_asset('videos')

print "Starting Norma..."


#a global variable
elapsedTime = 0.0
size = 200
WW = 640
HH = 480
content.FBO_SIZE = (WW,HH)
shader = ofShader()

content.add_parameter("hue", min=0.0, max=7.0, value=0.0)
content.add_parameter("gamma", min=0.0, max=1.0, value=0.99)
content.add_parameter("brightness", min=0.0, max=10.0, value=1.0)
content.add_parameter("contrast", min=0.0, max=10.0, value=1.0)
content.add_parameter("saturation", min=0.0, max=10.0, value=1.0)
content.add_parameter("position", min=0.0, max=1.0, value=0.0)
content.add_parameter('play', type='button')
content.add_parameter('next', type='button')
content.add_parameter('stop', type='button')


def setup():
    """
    This will be called at the beggining, you set your stuff here
    """

    global WW, HH, videoManager

    videoManager = videos.VideoManager(WW,HH)

    ofEnableAlphaBlending()

    setupShader()


def update():
    """
    For every frame, before drawing, we update stuff
    """
    #pass

    global videoManager

    videoManager.update()
    pos = videoManager.getPosition()
    if 0 <= pos <= 1:
        content['position'] = pos
        #print content['position']
 
        

def draw():
    """
    For every frame draw stuff. Do not forget to clear the frmebuffer!
    """
    #global shader, videoManager
    global shader

    ofClear(0)
    #videoManager.draw() 

    if shader.isLoaded():
        shader.begin()
        shader.setUniform3f("avgluma", 0.62,0.62,0.62)
        shader.setUniform1f('hue', content["hue"])
        shader.setUniform1f('gamma', content["gamma"])
        shader.setUniform1f('contrast', content["contrast"])
        shader.setUniform1f('saturation', content["saturation"])
        shader.setUniform1f('brightness', content["brightness"])
        
        ofClear(0)

        videoManager.draw()  

        shader.end()

  
def exit():
    """
    Before removing the script, in case you have pending business.
    """

    global videoManager
    videoManager.exit()
    #pass


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


@content.add_button('stop')
def stop():
    global videoManager
    videoManager.stop()

@content.add_button('play')
def play():
    global videoManager
    videoManager.play()

@content.add_button('next')
def next():
    global videoManager
    videoManager.nextVideo()

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

    const vec3 LumCoeff = vec3 (0.2125, 0.7154, 0.0721);
    const mat3 rgb2yiq = mat3(0.299, 0.587, 0.114, 0.595716, -0.274453, -0.321263, 0.211456, -0.522591, 0.311135);
    const mat3 yiq2rgb = mat3(1.0, 0.9563, 0.6210, 1.0, -0.2721, -0.6474, 1.0, -1.1070, 1.7046);

    uniform sampler2DRect texture0;
    uniform float contrast;
    uniform float saturation;
    uniform float brightness;
    uniform vec3 avgluma;
    uniform float hue;

    uniform float gamma;
    uniform float minInput = 0.0;
    uniform float maxInput = 1.0;
    uniform float minOutput = 0.0;
    uniform float maxOutput = 1.0;


    in vec2 texCoordVarying;

    out vec4 outputColor;
    """

    frag_contents = """


    /*
    ** Gamma correction
    ** Details: http://blog.mouaif.org/2009/01/22/photoshop-gamma-correction-shader/
    */
    #define GammaCorrection(color, gamma)                               vec3( pow(color.r, 1.0 / gamma), pow(color.g, 1.0 / gamma), pow(color.b, 1.0 / gamma) )
    /*
    ** Levels control (input (+gamma), output)
    ** Details: http://blog.mouaif.org/2009/01/28/levels-control-shader/
    */
    #define LevelsControlInputRange(color, minInput, maxInput)              min(max(color - vec3(minInput), vec3(0.0)) / (vec3(maxInput) - vec3(minInput)), vec3(1.0))
    #define LevelsControlInput(color, minInput, gamma, maxInput)                GammaCorrection(LevelsControlInputRange(color, minInput, maxInput), gamma)
    #define LevelsControlOutputRange(color, minOutput, maxOutput)           mix(vec3(minOutput), vec3(maxOutput), color)
    #define LevelsControl(color, minInput, gamma, maxInput, minOutput, maxOutput)   LevelsControlOutputRange(LevelsControlInput(color, minInput, gamma, maxInput), minOutput, maxOutput)

    /*
    ** Contrast, saturation, brightness
    ** Code of this function is from TGM's shader pack
    ** http://irrlicht.sourceforge.net/phpBB2/viewtopic.php?t=21057
    */
    // For all settings: 1.0 = 100% 0.5=50% 1.5 = 150%
    vec3 ContrastSaturationBrightness(vec3 color, float brt, float sat, float con)
    {
        // Increase or decrease theese values to adjust r, g and b color channels seperately
        const float AvgLumR = 0.5;
        const float AvgLumG = 0.5;
        const float AvgLumB = 0.5;
        
        const vec3 LumCoeff = vec3(0.2125, 0.7154, 0.0721);
        
        vec3 AvgLumin = vec3(AvgLumR, AvgLumG, AvgLumB);
        vec3 brtColor = color * brt;
        vec3 intensity = vec3(dot(brtColor, LumCoeff));
        vec3 satColor = mix(intensity, brtColor, sat);
        vec3 conColor = mix(AvgLumin, satColor, con);
        return conColor;
    }


            
    void main(){


        vec2 pos = texCoordVarying;
            
        vec4 colorIn = texture(texture0, pos);
        vec3 texColor = colorIn.rgb;
        float alpha = colorIn.a;

        vec3 yColor = rgb2yiq * texColor;
        float originalHue = atan(yColor.b, yColor.g);
        float finalHue = originalHue + hue;
        float chroma = sqrt(yColor.b*yColor.b+yColor.g*yColor.g);
        vec3 yFinalColor = vec3(yColor.r, chroma * cos(finalHue), chroma * sin(finalHue));
        vec3 hueColor = yiq2rgb*yFinalColor;

    
        // csb first
        outputColor.rgb = ContrastSaturationBrightness(hueColor, brightness, saturation, contrast);
        // levels
        outputColor.rgb = LevelsControl(outputColor.rgb, minInput, gamma, maxInput, minOutput, maxOutput);
        outputColor.a = alpha;
        
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






