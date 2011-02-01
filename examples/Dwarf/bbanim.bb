
Graphics3D 1024,768
MoveMouse 400,300

pivot=CreatePivot()
PositionEntity pivot,0,1.5,0

camera=CreateCamera( pivot )
CameraClsColor camera,100,100,100

PositionEntity camera,0,50,-100


AmbientLight 190, 190, 190


light2=CreateLight()
LightColor light2,120,120,220
RotateEntity light2,0,90,0
PositionEntity light2, 40,20,0


light3=CreateLight()
LightColor light3,120,120,130
RotateEntity light3,0,0,0
PositionEntity light3, -400,20,0


dude=LoadAnimMesh( "dwarf1.b3d" )


plane=CreatePlane()
PositionEntity plane, 0,.01,0
EntityAlpha plane,.5
mirror=CreateMirror()



;ExtractAnimSeq dude, 2, 14  ;walk
;ExtractAnimSeq dude, 16, 26  ;run
;ExtractAnimSeq dude, 28, 40  ;jump 1
;ExtractAnimSeq dude, 42, 54  ;jump 2 - for user controlled jumps


;ExtractAnimSeq dude, 56, 74  ;crouch - 56-59=down, 60-69=crouch, 70-74=get up

;ExtractAnimSeq dude, 75, 88  ;battle idle 1
;ExtractAnimSeq dude, 90, 110  ;battle idle 1


;ExtractAnimSeq dude, 112, 126  ;Attack 1 - swing
;ExtractAnimSeq dude, 128, 142  ;Attack 2 - Jump and overhead whack
;ExtractAnimSeq dude, 144, 160  ;Attack 3 - 360 backhand
;ExtractAnimSeq dude, 162, 180  ;Attack 4 - 2 Swipes
;ExtractAnimSeq dude, 182, 192  ;Attack 5 - Stab
;ExtractAnimSeq dude, 194, 210  ;Attack 6 - BLOCK

;ExtractAnimSeq dude, 212, 227  ;Die 1 - Forwards
;ExtractAnimSeq dude, 230, 251	;Die 1 - Backwards

;ExtractAnimSeq dude, 253, 272	;Nod YES
;ExtractAnimSeq dude, 274, 290	;Shake head NO

;ExtractAnimSeq dude, 292, 325	;Idle 1
;ExtractAnimSeq dude, 327, 360	;Idle 2


ExtractAnimSeq dude, 2, 360  ;All Animations



seq=1



While Not KeyHit(1)


If Not Animating( dude )
		Animate dude,1,.2,seq,1
		
	
	EndIf


If KeyDown( 203 )=True Then x#=x#-0.5
If KeyDown( 205 )=True Then x#=x#+0.5
If KeyDown( 208 )=True Then y#=y#-0.5
If KeyDown( 200 )=True Then y#=y#+0.5

If KeyDown( 205 )=True Then MoveEntity camera,1,0,0
If KeyDown( 203 )=True Then MoveEntity camera,-1,0,0
If KeyDown( 208 )=True Then MoveEntity camera,0,0,-1
If KeyDown( 200 )=True Then MoveEntity camera,0,0,1

x#=EntityX(camera)
y#=EntityY(camera)
z#=EntityZ(camera)
	

	
	Mouselook(camera)
;mouselook routine call

PositionEntity camera,x#,y#,z#

	
	UpdateWorld
	RenderWorld
	
	Text 0,0,"Mouse steers camera, cursors move camera"
	Flip
	
	If KeyHit(59) Then
SaveBuffer(FrontBuffer(),"grab"+ss+".bmp"):ss=ss+1
EndIf
;Press F1 For a screenshot
	
	
	
	
Wend

End




;Control
; Camera position, angle values
Global cam_x#,cam_z#,cam_pitch#,cam_yaw#,cam_speed#=.5		; Current
Global dest_cam_x#,dest_cam_z#,dest_cam_pitch#,dest_cam_yaw#	; Destination
Global ent_x#,ent_z#,ent_pitch#,ent_yaw#,ent_speed#=.5		; Current
Global dest_ent_x#,dest_ent_z#,dest_ent_pitch#,dest_ent_yaw#	; Destination

Function mouselook(camera)
	; Mouse look
	; ----------

	; Mouse x and y speed
	mxs=MouseXSpeed()
	mys=MouseYSpeed()
	
	; Mouse shake (total mouse movement)
	mouse_shake=Abs(((mxs+mys)/2)/1000.0)

	; Destination camera angle x and y values
	dest_cam_yaw#=dest_cam_yaw#-mxs
	dest_cam_pitch#=dest_cam_pitch#+mys

	; Current camera angle x and y values
	cam_yaw=cam_yaw+((dest_cam_yaw-cam_yaw)/5)
	cam_pitch=cam_pitch+((dest_cam_pitch-cam_pitch)/5)
	
	RotateEntity camera,cam_pitch#,cam_yaw#,0
	;RotateEntity camera,mxs,mys,0
		
	; Rest mouse position to centre of screen
	MoveMouse 400,300

		; Move camera using movement values
	MoveEntity camera,x#,y#,z#
		
End Function	









FreeEntity dude