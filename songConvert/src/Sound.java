import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

public class Sound {
    private final String name;
    private String instrumentBank;
    private String voice;
    private short volume;
    private boolean loop;
    private List<Note> notes;
    private short transpose;
    private short duration_offset;
    private static final String tab = "    ";

//    private static BiMap<String, Short> banks;
//    private static BiMap<String, Short> voices1;
//    private static BiMap<String, Short> voices2;
//
//    static {
//        banks = new HashBiMap.create();
//    }

    public Sound(String instrumentBank, String oldArray, String name) {
        this.instrumentBank = instrumentBank;
        oldArray = oldArray.replaceAll("(\\{|\\}|\\s|:|;)", "").replaceAll(",", " ");
        Scanner sc = new Scanner(oldArray);
        this.loop = true;
        this.voice = sc.next();
        this.volume = 127;
        this.voice = voice;
        short length = sc.nextShort();
        this.notes = new ArrayList<>();
        for (int i = 0; i < length; i++) {
            sc.nextShort(); // volume not used
            Note cur = new Note(sc.nextShort()/*pitch*/, sc.nextShort()/*duration*/);
            notes.add(cur);
        }
        this.transpose = 0;
        this.duration_offset = 0;
        this.name = name;
    }

    public String toString() {
        String out = "Sound Midi::" + name + "(\n";
        out += tab + instrumentBank + ",\n";
        out += tab + voice + ",\n";
        out += tab + volume + ",\n";
        out += tab + loop + ",\n";
        out += tab + notes.size() + ",\n";
        out += tab + "(uint8_t[]) {";
        for (int i = 0; i < notes.size(); i++) {
            out+= notes.get(i).getPitch();
            if(i != notes.size() - 1) {
                out+= ", ";
            }
        }
        out += "},\n";

        out += tab + "(uint8_t[]) {";
        for (int i = 0; i < notes.size(); i++) {
            out+= notes.get(i).getDuration();
            if(i != notes.size() - 1) {
                out+= ", ";
            }
        }
        out += "},\n";

        out += tab + transpose + ",\n";
        out += tab + duration_offset + ",\n";
        out += ");\n";
        return out;
    }


}
