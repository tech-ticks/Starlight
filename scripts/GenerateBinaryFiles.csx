// Execute with [path to SkyEditor Deluxe.dll] [ROM path] Scripts/GenerateBinaryFiles.csx
#load "../../../Stubs/RTDX.csx"

using System.Text;
using System.IO;
using SkyEditor.IO.Binary;
using SkyEditor.RomEditor.Domain.Rtdx;
using SkyEditor.RomEditor.Domain.Rtdx.Structures;
using SkyEditor.RomEditor.Domain.Rtdx.Structures.Executable;
using SkyEditor.RomEditor.Domain.Rtdx.Constants;
using SkyEditor.RomEditor.Infrastructure;

var pokemonDataInfos = Rom.GetPokemonDataInfo().Entries;
var nameToTextIdHash = Enum.GetValues(typeof(TextIDHash)).Cast<TextIDHash>()
 .ToDictionary(h => h.ToString("f"), h => (int)h);

var eosData = File.ReadAllLines("pokedex_numbers_and_descriptions.csv");

var sentryDutyFile = new BinaryFile(new MemoryStream());

void WriteFixedLengthString(string str, int length, Encoding encoding)
{
  int byteCount = encoding.GetByteCount(str);
  if (byteCount + 1 > length)
  {
    throw new Exception($"Input string '{str}' too long, must be shorter than {length}");
  }
  sentryDutyFile.WriteNullTerminatedString(sentryDutyFile.Position, encoding, str);
  for (int i = byteCount + encoding.GetByteCount(" "); i < length; i++)
  {
    sentryDutyFile.Write(sentryDutyFile.Position, 0);
  }
}

sentryDutyFile.WriteUInt32(sentryDutyFile.Length, (uint) eosData.Length);

foreach (var item in eosData)
{
  var split = item.Split(';');
  int pokedexNumber = int.Parse(split[0]);
  var descriptions = split.Skip(1).ToArray();

  if (descriptions.Length != 4)
  {
    throw new Exception("There must be four descriptions for each Pokémon.");
  }

  var infoIndex = pokemonDataInfos.FindIndex(info => info.PokedexNumber == pokedexNumber);
  if (infoIndex < 1)
    continue;

  string creatureSymbol = ((CreatureIndex) infoIndex).ToString();
  int nameHash = nameToTextIdHash["POKEMON_NAME__POKEMON_" + creatureSymbol];

  WriteFixedLengthString(pokedexNumber.ToString(), 8, Encoding.ASCII);
  WriteFixedLengthString(creatureSymbol.ToLower(), 32, Encoding.ASCII);
  sentryDutyFile.WriteInt32(sentryDutyFile.Length, nameHash);

  for (int i = 0; i < descriptions.Length; i++)
  {
    WriteFixedLengthString(descriptions[i], 96 * 2, Encoding.Unicode);
  }
}

var actorDatabase = Rom.GetMainExecutable().ActorDatabase;
actorDatabase.ActorDataList.Add(new PegasusActDatabase.ActorData
{
  SymbolName = "LOUDRED",
  PokemonIndex = CreatureIndex.DOGOOMU,
  FormType = PokemonFormType.NORMAL,
  WarehouseId = PokemonFixedWarehouseId.NULL
});

actorDatabase.WriteToBinaryFile("custom_data/actor_database.bin");
File.WriteAllBytes("custom_data/sentry_duty.bin", sentryDutyFile.ReadArray());
