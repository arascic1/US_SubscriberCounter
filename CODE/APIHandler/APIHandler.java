package ba.unsa.etf.us;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.lang.reflect.Array;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

public class APIHandler {

    private String APIKey = "AIzaSyCaBuzG403l9osSnlro-HzZW5Mll2711Vs";

    public String getAPIKey() {
        return APIKey;
    }

    public void setAPIKey(String APIKey) {
        this.APIKey = APIKey;
    }

    private ArrayList<String> cutIrrelevant(BufferedReader reader) {
        ArrayList<String> str = new ArrayList<String>();
        boolean relevant = false;
        for(String temp : reader.lines().collect(Collectors.toList())) {
            if(temp.contains("\"localized\": {")) {
                relevant = true;
            }

            if(relevant) {
                str.add(temp);
            }
        }

        return str;
    }

    private String procesiraj(ArrayList<String> str) {
        StringBuilder result = new StringBuilder();
        boolean countryDataPresent = false;
        for(String temp : str) {
            if(temp.contains("title")) {
                temp = temp.stripLeading().stripTrailing();
                result.append(temp, 10, temp.length() - 2);
                result.append("; ");
            }
            else if(temp.contains("country")) {
                countryDataPresent = true;
                temp = temp.stripLeading().stripTrailing();
                result.append(temp, 12, temp.length() - 1);
                result.append("; ");
            }
            else if(temp.contains("viewCount")) {
                if(!countryDataPresent) result.append("N/A; ");
                temp = temp.stripLeading().stripTrailing();
                result.append(temp, 14, temp.length() - 2);
                result.append("; ");
            }
            else if(temp.contains("subscriberCount")) {
                temp = temp.stripLeading().stripTrailing();
                result.append(temp, 20, temp.length() - 2);
                result.append("; ");
            }
            else if(temp.contains("videoCount")) {
                temp = temp.stripLeading().stripTrailing();
                result.append(temp, 15, temp.length() - 1);
                result.append(";");
            }
        }

        return result.toString();
    }

    public BufferedReader establishConnection(URL baseURL) {
        HttpURLConnection APIConn = null;
        try {
            APIConn = (HttpURLConnection) baseURL.openConnection();
            if (APIConn.getResponseCode() / 100 != 2) {
                throw new IOException("response code error");
            } else {
                InputStream response = APIConn.getInputStream();
                return new BufferedReader(new InputStreamReader(response, StandardCharsets.UTF_8));
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        return null;
    }

    public String getKanalFromUsername(String query) {
        try {
            URL baseURL = new URL("https://youtube.googleapis.com/youtube/v3/channels?part=snippet%2Cstatistics&" +
                            "forUsername=" + query +
                            "&key=" + APIKey);

            return procesiraj(cutIrrelevant(establishConnection(baseURL)));
        } catch (MalformedURLException e) {
            e.printStackTrace();
        }

        return null;
    }

    public String getKanalFromChannelID(String query) {
        try {
            URL baseURL = new URL("https://youtube.googleapis.com/youtube/v3/channels?" +
                    "part=snippet%2Cstatistics&" +
                    "id=" + query +
                    "&key=" + APIKey);

            return procesiraj(cutIrrelevant(establishConnection(baseURL)));
        } catch (MalformedURLException e) {
            e.printStackTrace();
        }

        return null;
    }
}
