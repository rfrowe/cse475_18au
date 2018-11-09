import java.lang.IllegalArgumentException;

public class Note {
    private short duration;

    public short getDuration() {
        return duration;
    }

    public Note setDuration(short duration) {
        this.duration = duration;
        return this;
    }

    public short getPitch() {
        return pitch;
    }

    public Note setPitch(short pitch) {
        this.pitch = pitch;
        return this;
    }

    private short pitch;

    public Note(short pitch, short duration) {
        //duration = (short)(duration / 1.57); // in case need to compress duration
        if (duration < 0 || duration > 255 || pitch < 0 || pitch > 255) {
            throw new IllegalArgumentException("duration and pitch must be unsigned bytes");
        }
        this.duration = duration;
        this.pitch = pitch;
    }


}
