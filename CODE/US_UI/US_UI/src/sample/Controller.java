package sample;

import javafx.event.ActionEvent;
import javafx.scene.control.*;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import java.nio.charset.StandardCharsets;

public class Controller {
    public Button addBtn;
    public Button delBtn;
    public Button exitBtn;
    public ListView<String> list;
    public TextField txt;
    public Label msg;
    private APIHandler handler = new APIHandler();
    private String topic = "ugradbeni/projekat/youtube";

    private void displayErrorMsg(String msg) {
        Alert errorAlert = new Alert(Alert.AlertType.ERROR);
        errorAlert.setHeaderText("Input not valid");
        errorAlert.setContentText(msg);
        errorAlert.showAndWait();
    }

    public void add(ActionEvent actionEvent) {
        String result = "";

        try {
            if (txt.getText().stripLeading().equals("")) {
                displayErrorMsg("Prazan query");
                return;
            } else if (txt.getText().strip().contains("youtube.com")) {
                // query je link
                if (txt.getText().strip().contains("/user/")) {
                    String[] temp = txt.getText().split("/");
                    result = handler.getKanalFromUsername(temp[temp.length - 1]);
                } else if (txt.getText().strip().contains("/channel/")) {
                    String[] temp = txt.getText().split("/");
                    result = handler.getKanalFromChannelID(temp[temp.length - 1]);
                } else {
                    displayErrorMsg("Neispravan unos");
                    return;
                }
            } else {
                result = handler.getKanalFromUsername(txt.getText().strip().replaceAll(" ", ""));
            }
        } catch (RuntimeException e) {
            displayErrorMsg("Nema rezultata za vaš upit");
            return;
        }

        String ime = result.split(";")[0];
        if(list.getItems().contains(ime)) {
            displayErrorMsg("Već dodan kanal");
        } else {
            byte[] payload = result.getBytes(StandardCharsets.UTF_8);
            list.getItems().add(ime);
            emitujMqttMsg(new MqttMessage(payload));
        }
    }

    public void delete(ActionEvent actionEvent) {
        if(list.getSelectionModel().selectedItemProperty() == null) {
            displayErrorMsg("Nije odabran kanal za brisanje");
        }
        else {
           byte[] payload = String.valueOf(list.getItems().indexOf(list.getSelectionModel().getSelectedItem()) + 1)
                   .getBytes(StandardCharsets.UTF_8);
           list.getItems().remove(list.getSelectionModel().getSelectedItem());
           emitujMqttMsg(new MqttMessage(payload));
        }
    }

    public void exit(ActionEvent actionEvent) {
        System.exit(0);
    }

    private void emitujMqttMsg(MqttMessage msg) {
        msg.setQos(0);
        msg.setRetained(true);
        try {
            Main.publisher.publish(topic, msg);
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }
}

