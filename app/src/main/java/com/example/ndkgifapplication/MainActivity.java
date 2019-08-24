package com.example.ndkgifapplication;

import android.Manifest;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.widget.ImageView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.PermissionChecker;

import java.io.File;
import java.util.ArrayList;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    private String[] denied;
    private String[] permissions = {Manifest.permission.WRITE_EXTERNAL_STORAGE};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            ArrayList<String> list = new ArrayList<>();
            for (int i = 0; i < permissions.length; i++) {
                if (PermissionChecker.checkSelfPermission(this, permissions[i]) == PermissionChecker.PERMISSION_DENIED) {
                    list.add(permissions[i]);
                }
            }
            if (list.size() != 0) {
                denied = new String[list.size()];
                for (int i = 0; i < list.size(); i++) {
                    denied[i] = list.get(i);
                    ActivityCompat.requestPermissions(this, denied, 5);
                }

            } else {
                init();
            }
        } else {
            init();
        }

    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if (requestCode == 5) {
            boolean isDenied = false;
            for (int i = 0; i < denied.length; i++) {
                String permission = denied[i];
                for (int j = 0; j < permissions.length; j++) {
                    if (permissions[j].equals(permission)) {
                        if (grantResults[j] != PackageManager.PERMISSION_GRANTED) {
                            isDenied = true;
                            break;
                        }
                    }
                }
            }
            if (isDenied) {
                Toast.makeText(this, "请开启权限", Toast.LENGTH_SHORT).show();
            } else {
                init();

            }
        }
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
    }

    private ImageView imageView;

    private GifHandler gifHandler;

    private Bitmap bitmap;
    private int index = 0;

    private final Object object = new Object();

    private boolean isStart = false;

    private void init() {
        imageView = findViewById(R.id.gif_view);

        String s = Environment.getExternalStorageDirectory().getAbsolutePath();
        final File file = new File(s + "/Download/image/", "zhenxiang.gif");
        if (file != null && file.exists()) {

            new Thread() {

                @Override

                public void run() {

                    super.run();

                    gifHandler = new GifHandler(file.getAbsolutePath());

                    int width = gifHandler.getWidth();

                    int height = gifHandler.getHeight();

                    bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);

                    while (index >= 0) {
                        int nextFrame = gifHandler.updateFrame(bitmap, index);
                        index++;
                        if (index >= gifHandler.getLength()) {
                            index = 0;
                        }
                        synchronized (object) {
                            try {
                                if (isStart) {
                                    object.wait(nextFrame);
                                } else {
                                    object.wait();
                                }
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                imageView.setImageBitmap(bitmap);
                            }
                        });
                    }
                }

            }.start();

        } else {

            Toast.makeText(this, "文件不存在", Toast.LENGTH_SHORT).show();

        }
    }

    public void startGif(View view) {
        if (!isStart) {
            isStart = true;
            synchronized (object) {
                object.notifyAll();
            }
        }
    }

    public void stopGif(View view) {
        if (isStart) {
            isStart = false;
        }
    }

}
