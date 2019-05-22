from openframeworks import *
from protopixel import Content
from random import randint
from os import listdir
from os.path import isfile, join
from numpy import random
import socket
import os
import datetime

class VideoManager:

    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.__initializeAttributes()
        self.__setPaths()
        #self.nextVideo()
        print "VideoManager::Intitialized"

    def __initializeAttributes(self):
        
        self.videoPaths = []
        self.videoNames = []
        self.videoPlayer = ofVideoPlayer()
        self.is_playing = True
        self.videoIndex = -1
        self.position = 0.0
        self.lastposition = 0.0
        self.isNight = True

    def __setPaths(self):
        full_path = os.path.realpath(__file__)
        path, filename = os.path.split(full_path)
        MASTER_PATH = path + "/videos"
        MASTER_PATH = "/Users/imanolgo/MEGA/Video/Norma"
        print MASTER_PATH
        EXTENSION = ".mp4"
        EXTENSION2 = ".mov"

        self.videoPaths = [MASTER_PATH+"/"+f for f in listdir(MASTER_PATH) if isfile(join(MASTER_PATH, f)) and (f.endswith(EXTENSION) or f.endswith(EXTENSION2))]
        self.videoNames = [f for f in listdir(MASTER_PATH) if isfile(join(MASTER_PATH, f)) and (f.endswith(EXTENSION) or f.endswith(EXTENSION2))]
        self.videoPaths.sort()
        self.videoNames.sort()

        for x in self.videoNames:
            print "VideoManager::__setPaths: video -> " + x

        for x in self.videoPaths:
            print "VideoManager::__setPaths: paths -> " + x


    def update(self):
        
        if self.videoPlayer.isLoaded() and self.is_playing:
            self.videoPlayer.update()
            self.position = self.videoPlayer.getPosition()
            if self.position == float('-inf'):
                if self.lastposition > 0:
                    self.position = 1.0
                else:
                    self.position = 0.0
            self.lastposition = self.position
            if self.position >= 0.99:
                self.lastposition = 0.0
                self.nextVideo()

    def nextVideo(self):
        self.videoIndex = (self.videoIndex + 1)%len(self.videoPaths)
        print "VideoManager::nextVideo: videoIndex -> " + str(self.videoIndex)
        path = self.videoPaths[self.videoIndex]
        name = self.videoNames[self.videoIndex]
        result = self.videoPlayer.load(path)
        print "VideoManager::nextVideo: loading -> " + path

        if result:
            self.videoPlayer.setLoopState(OF_LOOP_NONE)
            self.videoPlayer.play()
            self.is_playing = True
            print "VideoManager::nextVideo: isPlaying!!!! -> "

        else:
            print "VideoManager::nextVideo: couldn't load -> " + path

        
    def draw(self):
        
        #self.videoPlayer.draw(0, 0, self.width, self.height)
        if self.videoPlayer.isLoaded() and self.is_playing:
            self.videoPlayer.draw(0,0, self.width, self.height)
            
    def exit(self):
        self.videoPlayer.stop()
        self.videoPlayer.close()

    def play(self):
        self.videoIndex = self.videoIndex - 1 + len(self.videoPaths)
        self.nextVideo()

    def stop(self):
        if self.videoPlayer.isLoaded():
            self.videoPlayer.stop()
            self.is_playing = False

    def getPosition(self):
        return self.position
        
       



   
