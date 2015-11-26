package monbadge.monandroidtools;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.nfc.*;

public class SecurityGate extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        //NFC setup
        NfcManager manager = (NfcManager) this.getSystemService(this.NFC_SERVICE);
        NfcAdapter adapter = manager.getDefaultAdapter();

        AlertDialog.Builder builder1 = new AlertDialog.Builder(this);
        builder1.setMessage("");
        builder1.setCancelable(false);
        builder1.setNeutralButton("Okay", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                dialog.cancel();
                System.exit(0);
            }
        });

        if (adapter != null && !adapter.isEnabled())
        {
            AlertDialog alert = builder1.create();
            alert.setMessage("NFC is disabled! Please enable NFC to run this application.");
            alert.show();
        }

        else if (adapter == null)
        {
            AlertDialog alert = builder1.create();
            alert.setMessage("No NFC device detected!");
            alert.show();
        }


        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_security_gate);
        //Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        //setSupportActionBar(toolbar);

    }


    //Code to take care of hiding the red light image (awaiting the actual NFC code).

    

    @Override
    public void onBackPressed()
    {
        finish();
        System.exit(0);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_security_gate, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
