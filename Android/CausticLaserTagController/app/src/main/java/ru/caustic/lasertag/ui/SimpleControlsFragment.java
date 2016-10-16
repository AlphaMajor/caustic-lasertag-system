package ru.caustic.lasertag.ui;

import android.os.Bundle;
import android.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CheckBox;

import ru.caustic.lasertag.core.BridgeConnector;
import ru.caustic.lasertag.core.CausticController;
import ru.caustic.lasertag.core.DevicesManager;
import ru.caustic.lasertag.core.RCSProtocol;

public class SimpleControlsFragment extends Fragment {

    private Button buttonRespawn;
    private Button buttonKill;

    private CheckBox checkBoxControlRed;
    private CheckBox checkBoxControlBlue;
    private CheckBox checkBoxControlYellow;
    private CheckBox checkBoxControlGreen;
    private CheckBox checkBoxControlAllTeams;

    public SimpleControlsFragment()
    {
        super();
    }
    //private boolean workaroundDisableAutoChecking
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_simple_controls, container, false);

        buttonRespawn = (Button) view.findViewById(R.id.buttonRespawn);
        buttonKill = (Button) view.findViewById(R.id.buttonKill);

        checkBoxControlRed = (CheckBox) view.findViewById(R.id.checkBoxControlRed);
        checkBoxControlBlue = (CheckBox) view.findViewById(R.id.checkBoxControlBlue);
        checkBoxControlYellow = (CheckBox) view.findViewById(R.id.checkBoxControlYellow);
        checkBoxControlGreen = (CheckBox) view.findViewById(R.id.checkBoxControlGreen);
        checkBoxControlAllTeams = (CheckBox) view.findViewById(R.id.checkBoxControlAllTeams);

        checkBoxControlAllTeams.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                checkBoxControlAllTeamsClick(v);
            }
        });
        checkBoxControlRed.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                checkBoxAnyTeamClick(v);
            }
        });
        checkBoxControlBlue.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                checkBoxAnyTeamClick(v);
            }
        });
        checkBoxControlYellow.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                checkBoxAnyTeamClick(v);
            }
        });
        checkBoxControlGreen.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                checkBoxAnyTeamClick(v);
            }
        });

        buttonRespawn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                buttonRespawnClick(v);
            }
        });
        buttonKill.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                buttonKillClick(v);
            }
        });

        return view;
    }

    private void checkBoxControlAllTeamsClick(View v) {
        checkBoxControlRed.setChecked(checkBoxControlAllTeams.isChecked());
        checkBoxControlBlue.setChecked(checkBoxControlAllTeams.isChecked());
        checkBoxControlYellow.setChecked(checkBoxControlAllTeams.isChecked());
        checkBoxControlGreen.setChecked(checkBoxControlAllTeams.isChecked());
    }

    private void checkBoxAnyTeamClick(View v) {
        if (
                checkBoxControlRed.isChecked() &&
                checkBoxControlBlue.isChecked() &&
                checkBoxControlYellow.isChecked() &&
                checkBoxControlGreen.isChecked()
            )
            checkBoxControlAllTeams.setChecked(true);
        else
            checkBoxControlAllTeams.setChecked(false);
    }

    private int getSelectedTeams()
    {
        int result = 0;
        if (checkBoxControlRed.isChecked())
            result |= CausticController.BroadcastCalls.RED;
        if (checkBoxControlBlue.isChecked())
            result |= CausticController.BroadcastCalls.BLUE;
        if (checkBoxControlYellow.isChecked())
            result |= CausticController.BroadcastCalls.YELLOW;
        if (checkBoxControlGreen.isChecked())
            result |= CausticController.BroadcastCalls.GREEN;
        return result;
    }

    private void buttonRespawnClick(View v) {
        CausticController.getInstance().getBroadcastCalls().respawn(getSelectedTeams());
    }

    private void buttonKillClick(View v) {
        CausticController.getInstance().getBroadcastCalls().kill(getSelectedTeams());
    }
}