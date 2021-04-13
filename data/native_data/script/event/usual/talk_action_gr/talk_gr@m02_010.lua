-- <<★LuaBlob
-- x
-- x
-- x
-- x
-- x
-- x
-- x
-- x


-- <<★LuaBlob
-- x
-- x
-- x
-- <<★ScenePartitionBlob
{"label":"PARTNER","fadeSetting_":"Clear","partitionType_":"Normal"}
-- <<★LuaBlob
-- x
-- x
-- x
-- x
-- x
-- x


-- <<★LuaBlob
-- x
-- x
-- x
-- x
-- x
-- x
-- x
-- x
-- x
-- x
-- x
LuaWINDOW.DrawFace(FACE_POS.LEFT_X, FACE_POS.LEFT_Y, LuaSymAct("PARTNER"), FACE_TYPE.NORMAL)
-- x
LuaWINDOW.SwitchTalk({
  TYPE1={
    TextID("B01P01A_M01E02A_2N_01_0010","★NONE★",[[ -- Maybe this is it...
      もしかして　これは……。]]),
    TextID("B01P01A_M01E02A_2N_01_0020","★NONE★",[[ -- [hero], how about checking your mailbox?
      [hero]　ちょっとポストを
      {調|しら}べてみてよ……。]]),
  },
  -- -----------------------------------------
  DEFAULT={
    TextID("B01P01A_M01E02A_2N_01_0030","★NONE★",[[ -- Could this be?
      もしかして　これは……。]]),
    TextID("B01P01A_M01E02A_2N_01_0040","★NONE★",[[ -- [hero], will you check[R]the mailbox, please?
      [hero]　ちょっと　ポストを
      {調|しら}べてみて……。]]),
  },
  -- -----------------------------------------
})
-- x
LuaWINDOW.CloseMessage()

-- <<★ScenePartitionBlob
{"label":"LOUDRED","fadeSetting_":"Clear"}
-- <<★LuaBlob
-- x
-- x
-- x
-- x
-- x

-- <<★ActCommandBlob
CH:LOUDRED:{
  0::PlaySe:{"isWait":false,"symbol":"SE_EVT_SIGN_NOTICE_LOW_01","fadeInTime":0.0,"volume":1.0,"volumeSymbol":"DEFAULT"}
  1::PlayManpu:{"isWait":true,"symbol":"MP_EXCLAMATION"}
  2::PlaySe:{"isWait":false,"symbol":"SE_EVT_JUMP_02","fadeInTime":0.0,"volume":1.0,"":"DEFAULT"}
  3::FuncRef:{"refFuncName":"Jump2"}
}

-- <<★LuaBlob
LuaWINDOW.DrawFace(FACE_POS.LEFT_X, FACE_POS.LEFT_Y, LuaSymAct("LOUDRED"), FACE_TYPE.NORMAL)
LuaWINDOW.Talk(LuaSymAct("LOUDRED"),TextID("","★DEBUG★", "HEY! You two! Come here!"))
LuaWINDOW.CloseMessage()

-- <<★ActCommandBlob
CH:PARTNER:{
  0::PlayManpu:{"isWait":false,"symbol":"MP_SHOCK_R"}
  1::PlaySe:{"isWait":false,"symbol":"SE_EVT_SIGN_SHOCK_01","fadeInTime":0.0,"volume":1.0,"volumeSymbol":"DEFAULT"}
  2::SetFace:{"faceType":13,"eyeType":6,"mouthType":3}
  3::FuncRef:{"refFuncName":"JumpSurprise"}
  4::Sleep:{"timeSec":0.5}
}

-- <<★LuaBlob
LuaWINDOW.DrawFace(FACE_POS.LEFT_X, FACE_POS.LEFT_Y, LuaSymAct("PARTNER"), FACE_TYPE.PAIN)
LuaWINDOW.Talk(LuaSymAct("PARTNER"),TextID("","★DEBUG★","Wah! What is this voice?") )
LuaWINDOW.DrawFace(FACE_POS.LEFT_X, FACE_POS.LEFT_Y, LuaSymAct("PARTNER"), FACE_TYPE.FAINT)
LuaWINDOW.Talk(LuaSymAct("PARTNER"),TextID("","★DEBUG★","It's so loud![R]Ow, my ears hurt!") )
LuaWINDOW.CloseMessage()

-- <<★ActCommandBlob
*::{
  0::ScreenFade:{"moveCameraParam":{"distanceMode":0,"toCamParam":{"refSymbol":"","directData":{"fovType":105,"fieldOfView":60.0,"near":0.10000000149011612,"far":1000.0,"rotateType":100,"pos":{"x":0.0,"y":0.0,"z":0.0},"rotateQ":{"x":0.0,"y":0.0,"z":0.0,"w":0.0},"rotateAtPos":{"x":0.0,"y":0.0,"z":1.0},"rotateAtRollDeg":0.0}}},"isWait":true,"fadeMode":100,"fadeLayer":0,"fadeTime":0.5,"fadeTimeSymbol":"","fadeColorStart":{"r":0.0,"g":0.0,"b":0.0,"a":0.0},"fadeColorEnd":{"r":0.0,"g":0.0,"b":0.0,"a":0.0}}
  1::Sleep:{"timeSec":0.2}
}

-- <<★ActAssetBlob
ASSET:ActAsset_Chara:HERO:{"symbolName":"HERO","pos":{"posType":0,"pos":{"x":6.856,"y":0.0,"z":-1.607},"symbol":""},"rot":{"rotType":0,"onlyYaw":true,"rotQ":{"x":0.0,"y":-0.969835,"z":0.0,"w":0.2437623},"lookSymbol":""}}
ASSET:ActAsset_Chara:PARTNER:{"symbolName":"PARTNER","pos":{"posType":0,"pos":{"x":5.1,"y":0.0,"z":-1.607},"symbol":""},"rot":{"rotType":0,"onlyYaw":true,"rotQ":{"x":0.0,"y":-0.9507815,"z":0.0,"w":-0.3098624},"lookSymbol":""}}
ASSET:ActAsset_Chara:LOUDRED:{"symbolName":"LOUDRED","pos":{"posType":0,"pos":{"x":6,"y":0.0,"z":-2.9},"symbol":""},"rot":{"rotType":0,"onlyYaw":true,"rotQ":{"x":0.0,"y":0.0,"z":0.0,"w":1.0},"lookSymbol":""}}
MISC:{"groundMapId":"TOWN","comment":"","isAssetAddLoadAndContinue":false,"isAssetAddOnlyLoad":true,"appendAssetSetSymbols":[]}

-- <<★ActCommandBlob
CAMERA::{
  0::SetCam:{"cameraParam":{"refSymbol":"","directData":{"fovType":100,"fieldOfView":60.0,"near":2.0,"far":1000.0,"rotateType":100,"pos":{"x":6.252121,"y":3.9,"z":6.08406},"rotateQ":{"x":-0.002023936,"y":0.9800531,"z":-0.198474,"w":-0.009995501},"rotateAtPos":{"x":0.0,"y":0.0,"z":1.0},"rotateAtRollDeg":0.0}}}
}

*::{
  0::ScreenFade:{"moveCameraParam":{"distanceMode":0,"toCamParam":{"refSymbol":"","directData":{"fovType":100,"fieldOfView":60.0,"near":0.10000000149011612,"far":1000.0,"rotateType":100,"pos":{"x":6.001361,"y":3.161923,"z":1.488842},"rotateQ":{"x":-0.001817662,"y":0.9675597,"z":-0.2525398,"w":-0.006965171},"rotateAtPos":{"x":0.0,"y":0.0,"z":1.0},"rotateAtRollDeg":0.0}}},"isWait":true,"fadeMode":0,"fadeLayer":0,"fadeTime":0.5,"fadeTimeSymbol":"","fadeColorStart":{"r":0.0,"g":0.0,"b":0.0,"a":0.0},"fadeColorEnd":{"r":0.0,"g":0.0,"b":0.0,"a":0.0}}
}

-- <<★LuaBlob
LuaWINDOW.DrawFace(FACE_POS.LEFT_X, FACE_POS.LEFT_Y, LuaSymAct("LOUDRED"), FACE_TYPE.NORMAL)
LuaWINDOW.Talk(LuaSymAct("LOUDRED"),TextID("","★DEBUG★", "I'm Loudred from the Wigglytuff Guild."))
LuaWINDOW.Talk(LuaSymAct("LOUDRED"),TextID("","★DEBUG★", "You're wondering why I'm here?[R]I'm searching for new recruits, of course!"))
LuaWINDOW.CloseMessage()

-- <<★ActCommandBlob
CH:PARTNER:{
  0::PlaySe:{"isWait":false,"symbol":"SE_EVT_SIGN_QUESTION_01","fadeInTime":0.0,"volume":1.0,"volumeSymbol":"DEFAULT"}
  1::PlayManpu:{"isWait":true,"symbol":"MP_QUESTION"}
}

-- <<★LuaBlob
LuaWINDOW.DrawFace(FACE_POS.LEFT_X, FACE_POS.LEFT_Y, LuaSymAct("PARTNER"), FACE_TYPE.FAINT)
LuaWINDOW.Talk(LuaSymAct("PARTNER"),TextID("","★DEBUG★","A Jigglypuff shrouded in wonderment?[R]New roots?"))
LuaWINDOW.Talk(LuaSymAct("PARTNER"),TextID("","★DEBUG★","I can barely hear anything,[R]my ears are still ringing!"))
LuaWINDOW.CloseMessage()

-- <<★LuaBlob
LuaWINDOW.DrawFace(FACE_POS.LEFT_X, FACE_POS.LEFT_Y, LuaSymAct("LOUDRED"), FACE_TYPE.NORMAL)
LuaWINDOW.Talk(LuaSymAct("LOUDRED"),TextID("","★DEBUG★", "So, what do you say? You're gonna take my entrance exam, aren't you?!"))
LuaWINDOW.SelectWithCloseMessage({
  {text=TextID("","★DEBUG★","We're in!"),label="SEL_YES",default=true},
  {text=TextID("","★DEBUG★","We'll pass."),label="SEL_NO",},
})

-- <<★ActCommandBlob
CH:PARTNER:{
  0::SetFace:{"faceType":0,"eyeType":0,"mouthType":0}
}

-- <<★BranchLabelBlob
{"label":"SEL_YES"}

-- <<★ActCommandBlob
CH:LOUDRED:{
  0::SetFace:{"faceType":10,"eyeType":3,"mouthType":2}
  1::PlayManpu:{"isWait":false,"symbol":"MP_SPREE_LP"}
}

-- <<★LuaBlob
LuaWINDOW.DrawFace(FACE_POS.LEFT_X, FACE_POS.LEFT_Y, LuaSymAct("LOUDRED"), FACE_TYPE.HAPPY)
LuaWINDOW.Talk(LuaSymAct("LOUDRED"),TextID("","★DEBUG★", "Now that's what I like to hear!"))
LuaWINDOW.CloseMessage()

-- <<★ActCommandBlob
CH:LOUDRED:{
  0::SetFace:{"faceType":0,"eyeType":0,"mouthType":0}
  1::ResetManpu:{}
}

-- <<★LuaBlob
LuaWINDOW.DrawFace(FACE_POS.LEFT_X, FACE_POS.LEFT_Y, LuaSymAct("LOUDRED"), FACE_TYPE.NORMAL)
LuaWINDOW.Talk(LuaSymAct("LOUDRED"),TextID("","★DEBUG★", "Let's see if you're capable enough for sentry duty at the guild!"))
LuaWINDOW.Talk(LuaSymAct("LOUDRED"),TextID("","★DEBUG★", "That'll show me what you're made of!"))
LuaWINDOW.Talk(LuaSymAct("LOUDRED"),TextID("","★DEBUG★", "Identify the footprints as fast as you can![R]Now get to work!"))
LuaWINDOW.CloseMessage()

-- <<★ActCommandBlob
*::{
  0::StopBgm:{"isWait":true,"fadeTime":1.0,"channel":0}
  0::PlayBgm:{"symbol":"BGM_THE_GATEKEEPERS","fadeInTime":0.0,"isLoop":true,"channel":0,"volume":1.0,"volumeSymbol":"DEFAULT"}
  0::SpecialFunc:{"specialFuncType":3000,"branchLabelTrue":"","branchLabelFalse":""}
  0::Sleep:{"timeSec":100000000.0}
  0::StopBgm:{"isWait":true,"fadeTime":1.0,"channel":0}
  0::PlayBgm:{"symbol":"BGM_CAMP_HIROBA","fadeInTime":0.0,"isLoop":true,"channel":0,"volume":1.0,"volumeSymbol":"DEFAULT"}
}

-- <<★LuaBlob
LuaWINDOW.DrawFace(FACE_POS.LEFT_X, FACE_POS.LEFT_Y, LuaSymAct("LOUDRED"), FACE_TYPE.NORMAL)
LuaWINDOW.Talk(LuaSymAct("LOUDRED"),TextID("","★DEBUG★", "So, now you're gonna ask if you're hired, aren't you? Your results are..."))
LuaWINDOW.CloseMessage()

-- <<★LuaBlob
LuaWINDOW.DrawFace(FACE_POS.LEFT_X, FACE_POS.LEFT_Y, LuaSymAct("PARTNER"), FACE_TYPE.THINK)
LuaWINDOW.Talk(LuaSymAct("PARTNER"),TextID("","★DEBUG★","Uh... Sorry to interrupt you, but..."))
LuaWINDOW.DrawFace(FACE_POS.LEFT_X, FACE_POS.LEFT_Y, LuaSymAct("LOUDRED"), FACE_TYPE.NORMAL)
LuaWINDOW.Talk(LuaSymAct("LOUDRED"),TextID("","★DEBUG★","Huh?!"))
LuaWINDOW.DrawFace(FACE_POS.LEFT_X, FACE_POS.LEFT_Y, LuaSymAct("PARTNER"), FACE_TYPE.THINK)
LuaWINDOW.Talk(LuaSymAct("PARTNER"),TextID("","★DEBUG★","I'm sorry, but... You see..."))
LuaWINDOW.DrawFace(FACE_POS.LEFT_X, FACE_POS.LEFT_Y, LuaSymAct("PARTNER"), FACE_TYPE.NORMAL)
LuaWINDOW.Talk(LuaSymAct("PARTNER"),TextID("","★DEBUG★","[hero] and I are members of a rescue team,[R]so I'm afraid that we can't join your guild."))
LuaWINDOW.CloseMessage()

-- <<★ActCommandBlob
CH:LOUDRED:{
  0::PlaySe:{"isWait":false,"symbol":"SE_EVT_SIGN_SHOCK_01","fadeInTime":0.0,"volume":1.0,"volumeSymbol":"DEFAULT"}
  1::PlayManpu:{"isWait":true,"symbol":"MP_SHOCK_R"}
}

-- <<★LuaBlob
LuaWINDOW.DrawFace(FACE_POS.LEFT_X, FACE_POS.LEFT_Y, LuaSymAct("LOUDRED"), FACE_TYPE.NORMAL)
LuaWINDOW.Talk(LuaSymAct("LOUDRED"),TextID("","★DEBUG★","What?! A rescue team?"))
LuaWINDOW.DrawFace(FACE_POS.LEFT_X, FACE_POS.LEFT_Y, LuaSymAct("LOUDRED"), FACE_TYPE.NORMAL)
LuaWINDOW.Talk(LuaSymAct("LOUDRED"),TextID("","★DEBUG★","I see. Too bad![R]Time to search for another recruit."))
LuaWINDOW.Talk(LuaSymAct("LOUDRED"),TextID("","★DEBUG★","I'm afraid I'll have to stay here for a while,[R]so come back if you change your mind!"))
LuaWINDOW.CloseMessage()

-- <<★BranchLabelBlob
{"label":"SEL_NO"}

-- <<★ActCommandBlob
CH:LOUDRED:{
  0::PlaySe:{"isWait":false,"symbol":"SE_EVT_SIGN_ANGRY_02","fadeInTime":0.0,"volume":1.0,"volumeSymbol":"DEFAULT"}
  1::PlayManpu:{"isWait":false,"symbol":"MP_ANGRY_LP"}
  2::FuncRef:{"refFuncName":"Jump2"}
}

-- <<★LuaBlob
LuaWINDOW.DrawFace(FACE_POS.LEFT_X, FACE_POS.LEFT_Y, LuaSymAct("LOUDRED"), FACE_TYPE.NORMAL)
LuaWINDOW.Talk(LuaSymAct("LOUDRED"),TextID("","★DEBUG★","What?!"))
LuaWINDOW.CloseMessage()

-- <<★ActCommandBlob
CH:LOUDRED:{
  0::Sleep:{"timeSec":0.5}
  1::ResetManpu:{}
}

-- <<★LuaBlob
LuaWINDOW.DrawFace(FACE_POS.LEFT_X, FACE_POS.LEFT_Y, LuaSymAct("LOUDRED"), FACE_TYPE.NORMAL)
LuaWINDOW.Talk(LuaSymAct("LOUDRED"),TextID("","★DEBUG★","I see. Too bad![R]Time to search for another recruit!"))
LuaWINDOW.Talk(LuaSymAct("LOUDRED"),TextID("","★DEBUG★","I'm afraid I'll have to stay here for a while,[R]so come back if you change your mind!"))
LuaWINDOW.CloseMessage()

-- <<★BranchEndBlob

-- <<★ActCommandBlob
CAMERA::{
  0::MoveCamHero:{"isWait":true,"speed":10.0,"accelParam":{"startType":999,"endType":200,"startRatio":0.0,"endRatio":0.25}}
}
