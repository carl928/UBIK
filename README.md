# UpperBody IK (UBIK) #

UBIK is an Upper Body IK solution for VR inspired by Mathias Parger's master thesis http://diglib.tugraz.at/download.php?id=5c4a48dc5a282&location=browse
which was implemented in Unity and shared here: https://github.com/dabeschte/VRArmIK

## Features ##

* VR Upper Body based on two controllers and hmd
* 360deg movement
* Simple support for skeletal meshes
* Support for IK chain going from Pelvis and up
* Easy to use Plugin
* Bonus utility functions (RotatePointAroundPivot, SafeguardAngle, CosineRule, FindBetweenNormals)

### Requirements ###
* SkeletalMesh must be rigged with the default UE4 mannequin skeleton.

## Versionhistory ##
# 1.01 #
* Fix: Assume basepose in designtime, so you can see your mesh.
* Changed: Default LocalHandOffset (14,-2,0).

# 1.00 #
* First version that packages and has all the basic original BP features implemented.
* Missing feature: Upper and Lower wristbones are not implemented atm.




