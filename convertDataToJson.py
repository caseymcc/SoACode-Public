import os
import yaml
import json

def convertBlockTextureMapping(texturePackDirectory, outputDirectory):
    with open(texturePackDirectory+'BlockTextureMapping.yml') as inputFile:
        mapping=yaml.safe_load(inputFile)

    blockMapping={}
    blocks=[]
    
    blockMapping['version']="1.0"

    for name, value in mapping.items():
        block={}
        block['name']=name

        textureLocations={'texture', 'textureOpX', 'textureOpY', 'textureOpZ', 'textureTop', 'textureBotton', 'textureFront', 'textureBack', 'textureLeft', 'textureRight'}
        textureOptions={'base', 'overlay', 'blendMode'}

        if 'transparency' in value:
            block['transparency']=value['transparency']
        for textureLocation in textureLocations:
            if textureLocation in value:
                block[textureLocation]={}

                if isinstance(value[textureLocation], dict):
                    for textureOption in textureOptions:
                        if textureOption in value[textureLocation]:
                            block[textureLocation][textureOption]=value[textureLocation][textureOption]
                else:
                    block[textureLocation]['base']=value[textureLocation]

        blocks.append(block)

    blockMapping['blocks']=blocks

    with open(outputDirectory+'blockTextureMapping.json', 'w') as outfile:
        json.dump(blockMapping, outfile, indent=4)

def convertLayerProperties(texturePackDirectory, outputDirectory):
    yamlLayers={}
    with open(texturePackDirectory+'LayerProperties.yml') as inputFile:
        yamlLayers=yaml.safe_load(inputFile)
    
    textureOptions={'path', 'normalPath', 'dispPath', 'specularPath', 'method', 'innerSeams', 'color', 'altColors', 'coupling', 'weights'}

    layerMap={}
    
    layerMap['version']="1.0"
    layers=[]
    
    for name, value in yamlLayers.items():
        texture={}
        texture['name']=name
        
        for textureOption in textureOptions:
            if textureOption in value:
                texture[textureOption]=value[textureOption]
    
        layers.append(texture)
    
    layerMap['layers']=layers
    
    with open(outputDirectory+'layerProperties.json', 'w') as outfile:
        json.dump(layerMap, outfile, indent=4)

def convertPack(texturePackDirectory, outputDirectory):
    yamlPack={}
    with open(texturePackDirectory+'pack.yml') as inputFile:
        yamlPack=yaml.safe_load(inputFile)

    pack={}
    pack['version']="1.0"

    packOptions={'name', 'resolution', 'description'}
    
    for name, value in yamlPack.items():
        for packOption in packOptions:
            if packOption == name:
                pack[packOption]=value
    
    with open(outputDirectory+'pack.json', 'w') as outfile:
        json.dump(pack, outfile, indent=4)


def convertTextures(texturePackDirectory, outputDirectory):
    yamlTextures={}
    with open(texturePackDirectory+'Textures.yml') as inputFile:
        yamlTextures=yaml.safe_load(inputFile)
    
    textureMap={}
    textureMap['version']="1.0"

    textures=[]
    textureOptions={'base', 'overlay', 'blendMode'}

    index=0
    for name, value in yamlTextures.items():
        texture={}
        texture['name']=name
        
        for textureOption in textureOptions:
            if textureOption in value:
                texture[textureOption]=value[textureOption]
    
        textures.append(texture)
    
    textureMap['textures']=textures
    
    with open(outputDirectory+'textures.json', 'w') as outfile:
        json.dump(textureMap, outfile, indent=4)

def convertTexturePack(texturePackDirectory, outputDirectory):
    convertBlockTextureMapping(texturePackDirectory, outputDirectory)
    convertLayerProperties(texturePackDirectory, outputDirectory)
    convertPack(texturePackDirectory, outputDirectory)
    convertTextures(texturePackDirectory, outputDirectory)

def convertTexturePacks(dataDirectory, outputDirectory):
    texturePackDirectory=dataDirectory+'Textures/TexturePacks/'
    texturePackOutput=outputDirectory+'Textures/TexturePacks/'

    directories=[]
    for name in os.listdir(texturePackDirectory):
        if os.path.isdir(texturePackDirectory+name):
            directories.append(name)

    for directory in directories:
        directory=directory+'/'
        outputPath=texturePackOutput+directory

        if not os.path.exists(outputPath):
            os.makedirs(outputPath)

        convertTexturePack(texturePackDirectory+directory, outputPath)


dataDirectory='game/'
outputDirectory='gameJson/'

if not os.path.exists(outputDirectory):
    os.makedirs(outputDirectory)

convertTexturePacks(dataDirectory, outputDirectory)
