package monbadge.monandroidtools;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.media.Image;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.nfc.*;
import android.widget.ImageView;
import android.widget.Toast;

public class SecurityGate extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setLight(0);

        //NFC setup
        NfcManager manager = (NfcManager) this.getSystemService(this.NFC_SERVICE);
        NfcAdapter nfcAdapter = manager.getDefaultAdapter();
        
        if (nfcAdapter != null && !nfcAdapter.isEnabled())
        {
            Toast.makeText(SecurityGate.this, "NFC is disabled!", Toast.LENGTH_LONG).show();
        }

        else if (nfcAdapter == null)
        {
            Toast.makeText(SecurityGate.this, "No NFC module detected!", Toast.LENGTH_LONG).show();
        }

        else
        {
            Toast.makeText(SecurityGate.this, "NFC detected and enabled, nice!", Toast.LENGTH_LONG).show();
            setLight(1);
        }

        //Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        //setSupportActionBar(toolbar);
    }

    private void scanAccepted()
    {
        setLight(2);
        
        Thread timer = new Thread();
        try {
            timer.sleep(1000);
        }
        catch (InterruptedException e)
        {
            //EMPTY
        }
    }

    //Change the status light image.
    private void setLight(int setting)
    {
        setContentView(R.layout.content_security_gate);
        Integer images[] = {R.drawable.light_off,R.drawable.light_red,R.drawable.light_green};
        ImageView statusLight = (ImageView) findViewById(R.id.statusLight);

        statusLight.setImageResource(images[setting]);
    }

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
