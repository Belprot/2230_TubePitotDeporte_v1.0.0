package de.kai_morich.simple_bluetooth_terminal;

import static java.lang.Math.atan;
import static java.lang.Math.pow;
import static java.lang.Math.sqrt;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.res.ColorStateList;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.IBinder;
import android.text.Spannable;
import android.text.SpannableStringBuilder;
import android.text.style.ForegroundColorSpan;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import java.io.File;
import java.io.IOException;
import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.text.SimpleDateFormat;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.io.FileWriter;

import java.util.Date;
import java.util.List;
import java.util.Locale;


public class TerminalFragment extends Fragment implements ServiceConnection, SerialListener {

    private enum Connected { False, Pending, True }

    private String deviceAddress;
    private SerialService service;

    private TextView speedText;
    private TextView gyroXText;
    private TextView gyroYText;
    private TextView gyroZText;
    private TextView anglGyrXText;
    private TextView anglGyrYText;
    private TextView anglGyrZText;
    private TextView accelXText;
    private TextView accelYText;
    private TextView accelZText;
    private TextView vbatText;
    private TextView vgenText;
    private TextView pitchText;
    private TextView rollText;
    private TextView gravityForceText;
    private TextView sendText;

    private Button button_setZero;
    private Button button_logData;
    private Button button_reset;

    private ImageView imageView_Roll;
    private ImageView imageView_Pitch;

    private EditText editTextNumberDecimal_triggerPoint;
    private EditText editTextNumberDecimal_detectionDuration;

    private Switch switch_detectionMode;

    private TextUtil.HexWatcher hexWatcher;

    private Connected connected = Connected.False;
    private boolean initialStart = true;
    private boolean hexEnabled = false;
    private boolean logEnabled = false;
    private boolean fileCreated = false;
    private boolean isFreezed = false;
    private String newline = TextUtil.newline_crlf;
    private String fileName = "RPT_data.csv"; // Remote Pitot tube data

    private double sValue;
    private double axValue = 0.0;
    private double ayValue = 0.0;
    private double azValue = 0.0;
    private double vbatValue = 0.0;
    private double vgenValue = 0.0;
    private double gravityForce = 0;
    private double offset_pitch = 0;
    private double offset_roll = 0;
    private float pitch = 0;
    private float roll;
    private int counter= 0;

    /*
     * Lifecycle
     */
    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setHasOptionsMenu(true);
        setRetainInstance(true);
        deviceAddress = getArguments().getString("device");
    }

    @Override
    public void onDestroy() {
        if (connected != Connected.False)
            disconnect();
        getActivity().stopService(new Intent(getActivity(), SerialService.class));
        super.onDestroy();
    }

    @Override
    public void onStart() {
        super.onStart();
        if(service != null)
            service.attach(this);
        else
            getActivity().startService(new Intent(getActivity(), SerialService.class)); // prevents service destroy on unbind from recreated activity caused by orientation change
    }

    @Override
    public void onStop() {
        if(service != null && !getActivity().isChangingConfigurations())
            service.detach();
        super.onStop();
    }

    @SuppressWarnings("deprecation") // onAttach(context) was added with API 23. onAttach(activity) works for all API versions
    @Override
    public void onAttach(@NonNull Activity activity) {
        super.onAttach(activity);
        getActivity().bindService(new Intent(getActivity(), SerialService.class), this, Context.BIND_AUTO_CREATE);
    }

    @Override
    public void onDetach() {
        try { getActivity().unbindService(this); } catch(Exception ignored) {}
        super.onDetach();
    }

    @Override
    public void onResume() {
        super.onResume();
        if(initialStart && service != null) {
            initialStart = false;
            getActivity().runOnUiThread(this::connect);
        }
    }

    @Override
    public void onServiceConnected(ComponentName name, IBinder binder) {
        service = ((SerialService.SerialBinder) binder).getService();
        service.attach(this);
        if(initialStart && isResumed()) {
            initialStart = false;
            getActivity().runOnUiThread(this::connect);
        }
    }

    @Override
    public void onServiceDisconnected(ComponentName name) {
        service = null;
    }

    /*
     * UI
     */
    @SuppressLint("MissingInflatedId")
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_terminal, container, false);
        // Assignment text to textView
        speedText = view.findViewById(R.id.speed_viewer);
        //gyroXText = view.findViewById(R.id.gyroX_viewer);
        //gyroYText = view.findViewById(R.id.gyroY_viewer);
        //gyroZText = view.findViewById(R.id.gyroZ_viewer);
        //anglGyrXText = view.findViewById(R.id.angGyrX_viewer);
        //anglGyrYText = view.findViewById(R.id.angGyrY_viewer);
        //anglGyrZText = view.findViewById(R.id.angGyrZ_viewer);
        accelXText = view.findViewById(R.id.accelX_viewer);
        accelYText = view.findViewById(R.id.accelY_viewer);
        accelZText = view.findViewById(R.id.accelZ_viewer);
        vbatText = view.findViewById(R.id.vbat_viewer);
        vgenText = view.findViewById(R.id.vgen_viewer);
        pitchText = view.findViewById(R.id.pitch_viewer);
        //yawText = view.findViewById(R.id.yaw_viewer);
        rollText = view.findViewById(R.id.roll_viewer);
        gravityForceText = view.findViewById(R.id.gravityForce_viewer);

        button_setZero = view.findViewById(R.id.button_setZero);
        button_logData = view.findViewById(R.id.button_logData);
        button_reset = view.findViewById(R.id.button_reset);
        imageView_Roll = view.findViewById(R.id.imageView_Roll);
        imageView_Pitch = view.findViewById(R.id.imageView_Pitch);

        editTextNumberDecimal_triggerPoint = view.findViewById(R.id.editTextNumberDecimal_triggerPoint);
        editTextNumberDecimal_detectionDuration = view.findViewById(R.id.editTextNumberDecimal_detectionDuration);
        editTextNumberDecimal_triggerPoint.setText("0.95");
        editTextNumberDecimal_detectionDuration.setText("0.2");

        switch_detectionMode = view.findViewById(R.id.switch_detectionMode);

        sendText = view.findViewById(R.id.send_text);
        hexWatcher = new TextUtil.HexWatcher(sendText);
        hexWatcher.enable(hexEnabled);
        sendText.addTextChangedListener(hexWatcher);
        sendText.setHint(hexEnabled ? "HEX mode" : "");

        View sendBtn = view.findViewById(R.id.send_btn);
        sendBtn.setOnClickListener(v -> send(sendText.getText().toString()));


        button_setZero.setOnClickListener(new View.OnClickListener() {

            public void onClick(View v) {

                offset_pitch = - pitch;
                offset_roll = - roll;
            }
        });

        button_logData.setOnClickListener(new View.OnClickListener() {

            public void onClick(View v) {

                logEnabled = !logEnabled;

                if(logEnabled){

                    fileCreated = false;

                    // Obtenir la date et l'heure actuelles
                    Date currentDate = new Date();

                    // Format du nom de fichier avec la date et l'heure
                    SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd_HH-mm-ss", Locale.getDefault());
                    fileName = "RPT_data_" + dateFormat.format(currentDate) + ".csv";
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                        button_logData.setBackgroundTintList(ColorStateList.valueOf(Color.WHITE));
                    }
                } else {

                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                        button_logData.setBackgroundTintList(ColorStateList.valueOf(Color.DKGRAY));
                    }
                }
            }
        });

        button_reset.setOnClickListener(new View.OnClickListener() {

            public void onClick(View v) {

                isFreezed = false;
            }
        });

        return view;
    }

    @Override
    public void onCreateOptionsMenu(@NonNull Menu menu, MenuInflater inflater) {
        inflater.inflate(R.menu.menu_terminal, menu);
        menu.findItem(R.id.hex).setChecked(hexEnabled);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        if (id == R.id.clear) {
            //receiveText.setText("");
            return true;
        } else if (id == R.id.newline) {
            String[] newlineNames = getResources().getStringArray(R.array.newline_names);
            String[] newlineValues = getResources().getStringArray(R.array.newline_values);
            int pos = java.util.Arrays.asList(newlineValues).indexOf(newline);
            AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
            builder.setTitle("Newline");
            builder.setSingleChoiceItems(newlineNames, pos, (dialog, item1) -> {
                newline = newlineValues[item1];
                dialog.dismiss();
            });
            builder.create().show();
            return true;
        } else if (id == R.id.hex) {
            hexEnabled = !hexEnabled;
            sendText.setText("");
            hexWatcher.enable(hexEnabled);
            sendText.setHint(hexEnabled ? "HEX mode" : "");
            item.setChecked(hexEnabled);
            return true;
        } else {
            return super.onOptionsItemSelected(item);
        }
    }

    /*
     * Serial + UI
     */
    private void connect() {
        try {
            BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
            BluetoothDevice device = bluetoothAdapter.getRemoteDevice(deviceAddress);
            status("connecting...");
            connected = Connected.Pending;
            SerialSocket socket = new SerialSocket(getActivity().getApplicationContext(), device);
            service.connect(socket);
        } catch (Exception e) {
            onSerialConnectError(e);
        }
    }

    private void disconnect() {
        connected = Connected.False;
        service.disconnect();
    }

    private void send(String str) {
        if(connected != Connected.True) {
            Toast.makeText(getActivity(), "not connected", Toast.LENGTH_SHORT).show();
            return;
        }
        try {
            String msg;
            byte[] data;
            if(hexEnabled) {
                StringBuilder sb = new StringBuilder();
                TextUtil.toHexString(sb, TextUtil.fromHexString(str));
                TextUtil.toHexString(sb, newline.getBytes());
                msg = sb.toString();
                data = TextUtil.fromHexString(msg);
            } else {
                msg = str;
                data = (str + newline).getBytes();
            }
            SpannableStringBuilder spn = new SpannableStringBuilder(msg + '\n');
            spn.setSpan(new ForegroundColorSpan(getResources().getColor(R.color.colorSendText)), 0, spn.length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
            //receiveText.append(spn);
            service.write(data);
        } catch (Exception e) {
            onSerialIoError(e);
        }
    }



    private void receive(ArrayDeque<byte[]> datas) {
        List<Double> valeurs = new ArrayList<>();
        double[] a_dataReceived = new double[12];
        StringBuilder sb = new StringBuilder(); // Pour construire le message
        int tableauIndex = 0; // Compteur pour suivre l'indice dans le tableau
        // Chemin d'acces

        File downloadsDir = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS);
        File csvFile = new File(downloadsDir, fileName);

        Log.i(TerminalFragment.class.getName(),"chemin ::: " + csvFile);


        for (byte[] data : datas) {
            String msg = new String(data);

            // Ajouter le message au StringBuilder
            sb.append(msg);

            // Vérifier si le nombre de valeurs est un multiple de 12
            String[] values = sb.toString().trim().split("\\s+");
            if (values.length % 12 == 0) {
                // Le nombre de valeurs est un multiple de 12
                // Traiter le message en utilisant les valeurs

                for (String value : values) {
                    try {
                        double numericValue = Double.parseDouble(value);

                        if (tableauIndex >= a_dataReceived.length) {
                            // Gérer le cas où le tableau est plein (ou faites une action appropriée)
                            break; // Sort de la boucle pour arrêter de traiter les valeurs
                        }

                        a_dataReceived[tableauIndex] = numericValue;
                        tableauIndex++; // Incrémente le compteur d'indice

                    } catch (NumberFormatException e) {
                        // Gérer le cas où la valeur n'est pas un double valide
                    }
                }
                // Réinitialiser le StringBuilder pour le prochain message
                sb.setLength(0);

                // Sauvegrade des données dans les variables
                sValue += a_dataReceived[0];
                double gxValue = a_dataReceived[1];
                double gyValue = a_dataReceived[2];
                double gzValue = a_dataReceived[3];
                double gaxValue = a_dataReceived[4];
                double gayValue = a_dataReceived[5];
                double gazValue = a_dataReceived[6];
                axValue += a_dataReceived[7];
                ayValue += a_dataReceived[8];
                azValue += a_dataReceived[9];
                vbatValue += a_dataReceived[10];
                vgenValue += a_dataReceived[11];


                // Toutes les 5 mesures
                if(counter >= 5){

                    // Moyennage des données
                    sValue = sValue / 5;
                    axValue = axValue / 5;
                    ayValue = ayValue / 5;
                    azValue = azValue / 5;
                    vbatValue = vbatValue / 5;
                    vgenValue = vgenValue / 5;

                    // Conversion vitesse en km/h en noeud
                    double sValueKn = sValue * 0.539957;

                    // Calcul de la force de gravité pour chute libre
                    gravityForce = sqrt(pow(axValue, 2) + pow(ayValue, 2) + pow(azValue, 2));
                    // Vérifier la validité du texte (exemple : vérifier si c'est un nombre)
                    try {
                        float floatValue = Float.parseFloat(String.valueOf(editTextNumberDecimal_triggerPoint.getText()));
                        // Le texte est valide, faire ce que vous souhaitez avec la valeur floatValue
                    } catch (NumberFormatException e) {
                        // Le texte n'est pas valide, définir une valeur par défaut
                        String defaultValue = "0";
                        editTextNumberDecimal_triggerPoint.setText(defaultValue);
                    }


                    // Conversion des valeurs d'accélération de g à m/s²
                    float gravity = 9.81f; // Accélération due à la gravité en m/s²

                    float axValueInMetersPerSecondSquared = (float) (axValue * gravity);
                    float ayValueInMetersPerSecondSquared = (float) (ayValue * gravity);
                    float azValueInMetersPerSecondSquared = (float) (azValue * gravity);

                    // Calcul du Pitch
                    pitch = 0;
                    if (azValueInMetersPerSecondSquared != 0)
                        pitch = (float) atan(ayValueInMetersPerSecondSquared / azValueInMetersPerSecondSquared);

                    // Calcul du Roll
                    roll = 0;
                    if (azValueInMetersPerSecondSquared != 0)
                        roll = (float) atan(-axValueInMetersPerSecondSquared / azValueInMetersPerSecondSquared);

                    // Conversion des angles en degrés
                    float pitchDegrees = (float) ((float) Math.toDegrees(pitch + offset_pitch));
                    float rollDegrees = (float) ((float) Math.toDegrees(roll + offset_roll));

                    // Mise à jour de l'affichage
                    if(!isFreezed){
                        speedText.setText("SPEED    : " + String.format("%3.0f", sValueKn) + " KNOT");
                        accelXText.setText("ACCEL X  : " + String.format("%3.3f", axValue) + " g");
                        accelYText.setText("ACCEL Y  : " + String.format("%3.3f", ayValue) + " g");
                        accelZText.setText("ACCEL Z  : " + String.format("%3.3f", azValue) + " g");
                        pitchText.setText("PITCH   : " + String.format("%3.2f", pitchDegrees) + " °");
                        rollText.setText("ROLL     : " + String.format("%3.2f", rollDegrees) + " °");
                        vbatText.setText("VBAT     : " + String.format("%2.1f", vbatValue) + " V");
                        vgenText.setText("VGEN     : " + String.format("%2.1f", vgenValue) + " V");
                        gravityForceText.setText("GRAVITY VALUE : " + String.format("%3.3f", gravityForce) + " g");
                    }

                    // Si la valeur dépasse le seuil fixé, les valeurs sont freeze
                    if(gravityForce <= Float.parseFloat(String.valueOf(editTextNumberDecimal_triggerPoint.getText()))){

                        gravityForceText.setTextColor(Color.parseColor("#FF0000"));
                        if(switch_detectionMode.isChecked()) {
                            isFreezed = true;
                        }
                    } else {
                        if(!isFreezed) {
                            gravityForceText.setTextColor(Color.parseColor("#00FF00"));
                        }
                    }

                    // Mise à jour des images
                    imageView_Pitch.setRotation((float) -pitchDegrees);
                    imageView_Roll.setRotation((float) rollDegrees);

                    if(logEnabled) {
                        //logEnabled = false;

                        // Sauvegarde des données dans un fichier CSV
                        try {
                            FileWriter writer = new FileWriter(csvFile, true);

                            // Créez un format pour formater les nombres avec une virgule comme séparateur décimal
                            DecimalFormatSymbols symbols = new DecimalFormatSymbols(Locale.getDefault());
                            symbols.setDecimalSeparator(',');
                            DecimalFormat decimalFormat = new DecimalFormat("#0.00", symbols);

                            if(!fileCreated) {
                                fileCreated = true;
                                // En-têtes des colonnes
                                writer.append("SPEED [KNOT]; ACCEL X [g]; ACCEL Y [g]; ACCEL Z [g];" +
                                        "PITCH [degree]; ROLL [degree]; VBAT [V]; VGEN [V]; GRAVITY  [g]\n");
                            }
                            // Data
                            writer.append(decimalFormat.format(sValueKn) + ";" +
                                    decimalFormat.format(axValue) + ";" +
                                    decimalFormat.format(ayValue) + ";" +
                                    decimalFormat.format(azValue) + ";" +
                                    decimalFormat.format(pitchDegrees) + ";" +
                                    decimalFormat.format(rollDegrees) + ";" +
                                    decimalFormat.format(vbatValue) + ";" +
                                    decimalFormat.format(vgenValue) + ";" +
                                    decimalFormat.format(gravityForce) +"\n");

                            writer.flush();
                            writer.close();

                            Log.i(TerminalFragment.class.getName(),"WRITING OK");

                        } catch (IOException e) {
                            Log.i(TerminalFragment.class.getName(),"WRITING ERROR");
                            throw new RuntimeException(e);
                        }
                    }

                    // Mise à zéro des variables moyennées
                    sValue = 0;
                    axValue = 0;
                    ayValue = 0;
                    azValue = 0;
                    vbatValue = 0;
                    vgenValue = 0;

                    counter = 0;
                }
                counter++;
            }
        }
    }


    private void status(String str) {
        SpannableStringBuilder spn = new SpannableStringBuilder(str + '\n');
        spn.setSpan(new ForegroundColorSpan(getResources().getColor(R.color.colorStatusText)), 0, spn.length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
        //receiveText.append(spn);
    }

    /*
     * SerialListener
     */
    @Override
    public void onSerialConnect() {
        status("connected");
        connected = Connected.True;
    }

    @Override
    public void onSerialConnectError(Exception e) {
        status("connection failed: " + e.getMessage());
        disconnect();
    }

    @Override
    public void onSerialRead(byte[] data) {
        ArrayDeque<byte[]> datas = new ArrayDeque<>();
        datas.add(data);
        receive(datas);
    }

    public void onSerialRead(ArrayDeque<byte[]> datas) {
        receive(datas);
    }

    @Override
    public void onSerialIoError(Exception e) {
        status("connection lost: " + e.getMessage());
        disconnect();
    }
}


