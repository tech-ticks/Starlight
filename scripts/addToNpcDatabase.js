const { readFileSync, writeFileSync } = require('fs');

const contents = JSON.parse(readFileSync('custom_data/town_npc_database.json', 'utf-8'));

for (const data of contents.stepDataList_) {
  if (data.stepSymbol === 'm02_010') {
    data.npcDataList.push({
      "actSymbol": "LOUDRED",
      "refStepSymbol": "",
      "pos": {
        "posType": 0,
        "pos": {
          "x": 6,
          "y": 0,
          "z": -2.9
        },
        "symbol": ""
      },
      "rot": {
        "rotType": 0,
        "onlyYaw": true,
        "rotQ": {
          "x": 0,
          "y": 0,
          "z": 0,
          "w": 1
        },
        "lookSymbol": ""
      },
      "mapId": "TOWN",
      "redirectActSymbol": "",
      "actType": "",
      "actGroupId": "",
      "actAreaSymbol": "",
      "actSymbolSubNpc1": "",
      "actSymbolSubNpc2": "",
      "talkTurnType": 100,
      "isNoTalkNPC": false
    });
  } else if (data.stepSymbol.startsWith('m') || data.stepSymbol.startsWith('af')) {
    data.npcDataList.push({
      "actSymbol": "LOUDRED",
      "refStepSymbol": "m02_010",
      "pos": {
        "posType": 0,
        "pos": {
          "x": 6,
          "y": 0,
          "z": -2.9
        },
        "symbol": ""
      },
      "rot": {
        "rotType": 0,
        "onlyYaw": true,
        "rotQ": {
          "x": 0,
          "y": 0,
          "z": 0,
          "w": 1
        },
        "lookSymbol": ""
      },
      "mapId": "TOWN",
      "redirectActSymbol": "",
      "actType": "",
      "actGroupId": "",
      "actAreaSymbol": "",
      "actSymbolSubNpc1": "",
      "actSymbolSubNpc2": "",
      "talkTurnType": 100,
      "isNoTalkNPC": false
    });
  }
}

writeFileSync('custom_data/town_npc_database.json', JSON.stringify(contents, null, 2));
