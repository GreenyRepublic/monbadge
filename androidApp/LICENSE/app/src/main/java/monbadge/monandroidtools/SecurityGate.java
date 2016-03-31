//'Security Gate' activity: Turns a red light image to a green light image if a valid NFC tag is received.
//Written by Ben Clark with extensive help from Ralf Wondratschek's tutorial on code.tutsplus.com
//Link to Ralf's tutorial: http://code.tutsplus.com/tutorials/reading-nfc-tags-with-android--mobile-17278
//Appropriate pieces of code have been credited to Ralf.

package monbadge.monandroidtools;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.IntentFilter.MalformedMimeTypeException;
import android.nfc.tech.Ndef;
import android.os.Bundle;
import android.os.AsyncTask;
import android.util.Log;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.nfc.*;
import android.widget.ImageView;
import android.widget.Toast;

import java.io.UnsupportedEncodingException;
import java.util.Arrays;

public class SecurityGate extends AppCompatActivity {

    //NFC setup
    private NfcManager manager = (NfcManager) this.getSystemService(this.NFC_SERVICE);
    private NfcAdapter nfcAdapter = manager.getDefaultAdapter();
    public static final String MIME_TEXT_PLAIN = "text/plain";
    private static final String TAG = SecurityGate.class.getSimpleName();

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        //Initial stuff
        super.onCreate(savedInstanceState);
        setLight(0);

        //Display a message and set the light appropriately depending on the NFC adapter state.
        if (nfcAdapter == null)
        {
            Toast.makeText(SecurityGate.this, "No NFC module detected!", Toast.LENGTH_LONG).show();
            finish();
            return;
        }

        else if (nfcAdapter != null && !nfcAdapter.isEnabled())
        {
            Toast.makeText(SecurityGate.this, "NFC is disabled!", Toast.LENGTH_LONG).show();
            setLight(0);
        }

        else
        {
            Toast.makeText(SecurityGate.this, "NFC detected and enabled!", Toast.LENGTH_LONG).show();
            setLight(1);
        }

        handleIntent(getIntent());

        //Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        //setSupportActionBar(toolbar);
    }

    //If a valid NFC transmission is received, turn the light green for a second and a half.
    private void scanAccepted()
    {
        setLight(2);
        Thread timer = new Thread();
        try {
            timer.sleep(1500);
        }
        catch (InterruptedException e)
        {
            //EMPTY
        }
        setLight(1);
    }

    //Change the status light image.
    private void setLight(int setting)
    {
        setContentView(R.layout.content_security_gate);
        Integer images[] = {R.drawable.light_off,R.drawable.light_red,R.drawable.light_green};
        ImageView statusLight = (ImageView) findViewById(R.id.statusLight);

        statusLight.setImageResource(images[setting]);
    }

    //Get data from the NFC tag.
    //Credit to Ralf Wondratschek.
    private void handleIntent(Intent intent)
    {
        String data = intent.getAction();
        if (nfcAdapter.ACTION_NDEF_DISCOVERED.equals(data))
        {
            String type = intent.getType();
            if (MIME_TEXT_PLAIN.equals(type))
            {
                Tag tag = intent.getParcelableExtra(nfcAdapter.EXTRA_TAG);
                new NdefReaderTask().execute(tag);
            }
            else
            {
                Log.d(TAG, "Wrong MIME type: " + type);
            }
        }
    }

    //Get data from the NFC tag.
    //Credit to Ralf Wondratschek.
    private class NdefReaderTask extends AsyncTask<Tag, Void, String>
    {
        @Override
        protected String doInBackground(Tag... params)
        {
            Tag tag = params[0];
            Ndef ndef = Ndef.get(tag);

            NdefMessage ndefMessage = ndef.getCachedNdefMessage();
            NdefRecord[] records = ndefMessage.getRecords();
            for(NdefRecord ndefRecord : records)
            {
                if (ndefRecord.getTnf() == NdefRecord.TNF_WELL_KNOWN && Arrays.equals(ndefRecord.getType(), NdefRecord.RTD_TEXT))
                {
                    try {
                        return readText(ndefRecord);
                    }
                    catch(UnsupportedEncodingException e)
                    {
                        Log.e(TAG, "Unsupported Encoding", e);
                    }
                }
            }
            return null;
        }

        private String readText(NdefRecord record) throws UnsupportedEncodingException {
        /*
         * See NFC forum specification for "Text Record Type Definition" at 3.2.1
         *
         * http://www.nfc-forum.org/specs/
         *
         * bit_7 defines encoding
         * bit_6 reserved for future use, must be 0
         * bit_5..0 length of IANA language code
         */

            byte[] payload = record.getPayload();

            // Get the Text Encoding
            String textEncoding;
            if ((payload[0] & 128) == 0) textEncoding = "UTF-8";
            else textEncoding = "UTF-16";

            // Get the Language Code
            int languageCodeLength = payload[0] & 0063;

            // String languageCode = new String(payload, 1, languageCodeLength, "US-ASCII");
            // e.g. "en"

            // Get the Text
            return new String(payload, languageCodeLength + 1, payload.length - languageCodeLength - 1, textEncoding);
        }

        @Override
        protected void onPostExecute(String result) {
            if (result != null) {
                setLight(2);
            }
        }
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


    //The following few functions are used for handling app resumption and preventing nasty errors that may result from it.
    //Credit to Ralf Wondratschek.
    //http://tutsplus.com/authors/ralf-wondratschek

    @Override
    protected void onResume() {
        super.onResume();

        /**
         * It's important, that the activity is in the foreground (resumed). Otherwise
         * an IllegalStateException is thrown.
         */
        setupForegroundDispatch(this, nfcAdapter);
    }

    @Override
    protected void onPause() {
        /**
         * Call this before onPause, otherwise an IllegalArgumentException is thrown as well.
         */
        stopForegroundDispatch(this, nfcAdapter);

        super.onPause();
    }

    public static void setupForegroundDispatch(final Activity activity, NfcAdapter adapter) {
        final Intent intent = new Intent(activity.getApplicationContext(), activity.getClass());
        intent.setFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);

        final PendingIntent pendingIntent = PendingIntent.getActivity(activity.getApplicationContext(), 0, intent, 0);

        IntentFilter[] filters = new IntentFilter[1];
        String[][] techList = new String[][]{};

        // Notice that this is the same filter as in our manifest.
        filters[0] = new IntentFilter();
        filters[0].addAction(NfcAdapter.ACTION_NDEF_DISCOVERED);
        filters[0].addCategory(Intent.CATEGORY_DEFAULT);
        try {
            filters[0].addDataType(MIME_TEXT_PLAIN);
        } catch (MalformedMimeTypeException e) {
            throw new RuntimeException("Check your MIME type.");
        }

        adapter.enableForegroundDispatch(activity, pendingIntent, filters, techList);
    }

    public static void stopForegroundDispatch(final Activity activity, NfcAdapter adapter) {
        adapter.disableForegroundDispatch(activity);
    }
}
