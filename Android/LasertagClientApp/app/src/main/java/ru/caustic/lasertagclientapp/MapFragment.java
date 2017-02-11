package ru.caustic.lasertagclientapp;


import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.drawable.ShapeDrawable;
import android.graphics.drawable.shapes.OvalShape;
import android.os.Bundle;
import android.os.Handler;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.support.v4.app.FragmentTransaction;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.LatLngBounds;
import com.google.android.gms.maps.model.MapStyleOptions;
import com.google.android.gms.maps.model.MarkerOptions;

import java.util.ArrayList;
import java.util.Map;
import java.util.Set;

import ru.caustic.rcspcore.BridgeConnector;
import ru.caustic.rcspcore.CausticController;
import ru.caustic.rcspcore.DeviceUtils;
import ru.caustic.rcspcore.DevicesManager;
import ru.caustic.rcspcore.GameStatistics;
import ru.caustic.rcspcore.RCSProtocol;

import static ru.caustic.rcspcore.DeviceUtils.getCurrentHealth;
import static ru.caustic.rcspcore.DeviceUtils.getDeviceName;
import static ru.caustic.rcspcore.DeviceUtils.getDeviceTeam;
import static ru.caustic.rcspcore.DeviceUtils.getMarkerColor;


/**
 * A simple {@link Fragment} subclass.
 */
public class MapFragment extends Fragment  implements OnMapReadyCallback {


    public MapFragment() {
        // Required empty public constructor
    }


    private static DevicesManager devMan = CausticController.getInstance().getDevicesManager();
    private static GameStatistics gameStats = CausticController.getInstance().getGameStatistics();
    private static ArrayList<PlayerOnMap> playerOnMapList = new ArrayList<>();
    private static GoogleMap map;
    private static final String TAG = "HUDFragment";
    private static DevicesManager.CausticDevice headSensor = devMan.devices.get(devMan.associatedHeadSensorAddress);

    private static final int MARKER_BORDER_WIDTH_SP = 4;
    private static final int MARKER_RADIUS_SP = 20;
    private static final int STATE_UPDATE_DELAY_MS = 3000;
    private static boolean devStateUpdaterRunning = false;

    private static DevicesManager.SynchronizationEndListener mListener;

    private Handler devStateUpdateHandler = new Handler();
    private static Runnable devStateUpdater = new Runnable(){
        @Override
        public void run() {
            devMan.invalidateDevsMapParams(DeviceUtils.getHeadSensorsMap(devMan.devices).values());
            devMan.asyncPopParametersFromDevices(mListener, DeviceUtils.getHeadSensorsMap(devMan.devices).keySet());
            gameStats.updateStats();
        }
    };

    private DevicesManager.SynchronizationEndListener synchronizationEndListener = new DevicesManager.SynchronizationEndListener() {
        @Override
        public void onSynchronizationEnd(boolean isSuccess, Set<BridgeConnector.DeviceAddress> notSynchronized) {

            Log.d(TAG, "Sync completed, result: " + isSuccess + ", not synchronized: " + notSynchronized.size());

            playerOnMapList = populatePlayerOnMapList(DeviceUtils.getHeadSensorsMap(devMan.devices));
            FragmentActivity activity=getActivity();
            if (activity!=null)
            {
                activity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        refreshMarkersOnMap();
                        Log.d(TAG, "Updating markers");
                    }
                });
            }
            devStateUpdateHandler.postDelayed(devStateUpdater, STATE_UPDATE_DELAY_MS);
        }

    };

    private GameStatistics.StatsChangeListener statsUpdatedListener = new GameStatistics.StatsChangeListener() {

        @Override
        public void onStatsChange(int victimId, int damagerId)
        {
            final FragmentActivity activity=getActivity();
            if (activity!=null) {
                activity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        TextView kills = (TextView) activity.findViewById(R.id.kill_count);
                        TextView score = (TextView) activity.findViewById(R.id.score_count);
                        TextView deaths = (TextView) activity.findViewById(R.id.death_count);
                        int killCount = 0;
                        int friendlyKills = 0;
                        int scoreCount = 0;
                        int deathCount = 0;
                        int hitCount = 0;
                        if (headSensor!=null) {
                            int ownId = Integer.parseInt(headSensor.parameters.get(RCSProtocol.Operations.HeadSensor.Configuration.playerGameId.getId()).getValue());
                            killCount = gameStats.getStatForPlayerID(ownId, GameStatistics.ENEMY_KILLS_COUNT);
                            hitCount = gameStats.getStatForPlayerID(ownId, GameStatistics.HITS_COUNT);
                            deathCount = Integer.parseInt(headSensor.parameters.get(RCSProtocol.Operations.HeadSensor.Configuration.deathCount.getId()).getValue());
                            friendlyKills = gameStats.getStatForPlayerID(ownId, GameStatistics.FRIENDLY_KILLS_COUNT);
                            scoreCount = 2*killCount + hitCount - deathCount - 2*friendlyKills;
                            if (kills!=null) kills.setText(Integer.toString(killCount));
                            if (score!=null) score.setText(Integer.toString(scoreCount));
                            if (deaths!=null) deaths.setText(Integer.toString(deathCount));
                            Log.d(TAG, "Updating stats");
                        }
                    }
                });
            }
        }
    };

    private void refreshMarkersOnMap() {
        if (map!=null) {
            map.clear();
            for (PlayerOnMap player : playerOnMapList) {
                Log.d(TAG, "Setting marker at " + player.lat + ", " + player.lon);
                LatLng latLng = new LatLng(player.lat, player.lon);
                //map.moveCamera(CameraUpdateFactory.newLatLng(latLng));
                Bitmap icon = createMarkerIcon(getActivity(), player.markerColor, player.team, player.isAlive);
                MarkerOptions options = new MarkerOptions()
                        .position(latLng).title(player.name).anchor(0.5f, 0.5f).icon(BitmapDescriptorFactory.fromBitmap(icon));
                map.addMarker(options);
            }
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View layout = inflater.inflate(R.layout.fragment_map, container, false);

        Map<BridgeConnector.DeviceAddress, DevicesManager.CausticDevice> devsToLocate = DeviceUtils.getHeadSensorsMap(devMan.devices);
        playerOnMapList = populatePlayerOnMapList(devsToLocate);

        Fragment mMapFragment = SupportMapFragment.newInstance();
        FragmentTransaction fragmentTransaction =
                getChildFragmentManager().beginTransaction();
        fragmentTransaction.add(R.id.map_fragment_container, mMapFragment);
        fragmentTransaction.commit();

        //Need to call getMapAsync to receive a OnMapReady callback
        SupportMapFragment mapFrag = (SupportMapFragment) mMapFragment;
        mapFrag.getMapAsync(this);

        Intent intent = new Intent(getActivity(), HeadSensorLocationUpdateService.class);

        getActivity().startService(intent);
        return layout;
    }

    @Override
    public void onMapReady(GoogleMap googleMap) {
        map = googleMap;
        FragmentActivity activity = getActivity();
        if (activity!=null) {
            map.setMapStyle(
                    MapStyleOptions.loadRawResourceStyle(
                            activity, R.raw.map_style_json));
        }
        refreshMarkersOnMap();
        moveCameraToMarkerBounds(map, playerOnMapList);
    }

    private void moveCameraToMarkerBounds(GoogleMap map, ArrayList<PlayerOnMap> playerOnMapList) {
        LatLngBounds.Builder builder = LatLngBounds.builder();
        int includedPoints =0;
        for (PlayerOnMap player : playerOnMapList) {
            if (player.isVisible) {
                LatLng latLng = new LatLng(player.lat, player.lon);
                builder.include(latLng);
                includedPoints++;
            }
        }
        if (includedPoints>0) {
            LatLngBounds bounds = builder.build();
            map.moveCamera(CameraUpdateFactory.newLatLngBounds(bounds, 50));
        }
    }

    public static Bitmap createMarkerIcon(Context context, int markerColor, String team, boolean isAlive) {

        int pxRadius = MARKER_RADIUS_SP * Math.round(context.getResources().getDisplayMetrics().scaledDensity);
        int pxBorderWidth = MARKER_BORDER_WIDTH_SP * Math.round(context.getResources().getDisplayMetrics().scaledDensity);


        Bitmap b = Bitmap.createBitmap(pxRadius, pxRadius, Bitmap.Config.ARGB_8888);
        Canvas c = new Canvas(b);
        ShapeDrawable mDrawable = new ShapeDrawable(new OvalShape());

        //Drawing circle with team color first (if alive) and then a smaller circle with marker color second

        if (isAlive) {
            int teamColor = getTeamColor(context, team);
            mDrawable.getPaint().setColor(teamColor);
            mDrawable.setBounds(0, 0, c.getWidth(), c.getHeight());
            mDrawable.draw(c);
        }

        mDrawable.getPaint().setColor(markerColor);
        mDrawable.setBounds(pxBorderWidth, pxBorderWidth, c.getWidth()-pxBorderWidth, c.getHeight()-pxBorderWidth);
        mDrawable.draw(c);

        return b;
    }

    private static int getTeamColor(Context context, String team) {
        switch (team)
        {
            case "Red":
                return ContextCompat.getColor(context, R.color.red);
            case "Blue":
                return ContextCompat.getColor(context, R.color.blue);
            case "Green":
                return ContextCompat.getColor(context, R.color.green);
            case "Yellow":
                return ContextCompat.getColor(context, R.color.yellow);
            default:
                return ContextCompat.getColor(context, R.color.grey);
        }

    }

    private class PlayerOnMap
    {
        float lat=0;
        float lon=0;
        String name = "Unknown";
        String team = "Unknown";
        int markerColor = 0xFFFFFFFF;
        boolean isVisible = true;
        boolean isAlive = true;
        public PlayerOnMap(DevicesManager.CausticDevice device)
        {

            if (device.areDeviceRelatedParametersAdded()&&device.areParametersSync())
            {
                lat = Float.parseFloat(device.parameters.get(RCSProtocol.Operations.HeadSensor.Configuration.playerLat.getId()).getValue());
                lon = Float.parseFloat(device.parameters.get(RCSProtocol.Operations.HeadSensor.Configuration.playerLon.getId()).getValue());
                if (lat>90 || lat<-90 || lon>180 || lon<-180) isVisible = false;
                team = getDeviceTeam(device);
                name = getDeviceName(device);
                markerColor = getMarkerColor(device);
                if (getCurrentHealth(device)==0) isAlive = false;
            }
        }
    }


    @Override
    public void onStart() {
        super.onStart();
        if (!devStateUpdaterRunning) {
            startDevStateUpdater();
        }
    }

    @Override
    public void onStop() {
        super.onStop();
        if (devStateUpdaterRunning) stopDevStateUpdater();
    }

    private void startDevStateUpdater() {
        mListener=synchronizationEndListener;
        devStateUpdateHandler.post(devStateUpdater);
        devStateUpdaterRunning = true;
    }

    private void stopDevStateUpdater() {
        devStateUpdateHandler.removeCallbacks(devStateUpdater);
        devStateUpdaterRunning = false;
    }

    private ArrayList<PlayerOnMap> populatePlayerOnMapList(Map<BridgeConnector.DeviceAddress, DevicesManager.CausticDevice> headSensorsMap) {
        ArrayList<PlayerOnMap> returnList = new ArrayList<>();
        for (Map.Entry<BridgeConnector.DeviceAddress, DevicesManager.CausticDevice> entry : headSensorsMap.entrySet()) {

            DevicesManager.CausticDevice dev = entry.getValue();
            PlayerOnMap newPlayer = new PlayerOnMap(dev);
            returnList.add(newPlayer);
        }
        return returnList;
    }


}