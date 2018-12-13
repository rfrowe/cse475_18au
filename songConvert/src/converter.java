public class converter {
    public static void main(String[] args) {

        // Replace oldNoteArray with your own from the old code
        // note that {} and whitespace are ignored, so you can have or not
        // just be sure no extra letters or numbers are here
        String oldNoteArray = "{8,8,\n" +
                "   127,12,80,  127,24,80,  127,36,80,  127,48,80,  \n" +
                "   127,60,80,  127,72,80,  127,84,80,  127,96,80   },  \n";
        Sound s = new Sound("VS1053_BANK_MELODY", oldNoteArray,"five_startle");
        System.out.println(s);
    }
}
