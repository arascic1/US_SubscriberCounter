package ba.unsa.etf.us;

public class Main {
    public static void main(String[] args) {
        APIHandler API = new APIHandler();
        System.out.println(API.getKanalFromUsername("MotoGP"));
        System.out.println(API.getKanalFromChannelID("UCq0j6FS1V1mit68TtRFxQUQ"));
    }
}
