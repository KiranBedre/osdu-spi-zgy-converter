The files in this folder are sample parameter files intended to be used together with corresponding OSDU Volve test data sets
	ST10010ZC11_PZ_PSDM_KIRCH_FULL_D.MIG_FIN.POST_STACK.3D.JS-017536.segy (Volve.D.FileCollection.SEGY.json)
and
	ST10010ZC11_PZ_PSDM_KIRCH_FULL_T.MIG_FIN.POST_STACK.3D.JS-017536.segy (Volve.T.FileCollection.SEGY.json)


In order for this to work correctly, the path to the .segy files should be updated in the "FileSourceInfos" section:
e.g. from line number 40:
"FileSourceInfos": [
        {
          "FileSource": "E:\\Seismic\\SEGY 3D\\Volve\\ST10010ZC11_PZ_PSDM_KIRCH_FULL_D.MIG_FIN.POST_STACK.3D.JS-017536.segy",      
          "PreloadFilePath": "MyDocuments\\seismic-to-osdu\\VOLVE_DEPTH\\ST10010ZC11_PZ_PSDM_KIRCH_FULL_D.MIG_FIN.POST_STACK.3D.JS-017536.segy",
          "Name": "VOLVE_DEPTH",
          "FileSize": "1368413160"
        }
      ],



The line 
          "FileSource": "E:\\Seismic\\SEGY 3D\\Volve\\ST10010ZC11_PZ_PSDM_KIRCH_FULL_D.MIG_FIN.POST_STACK.3D.JS-017536.segy",      

must be updated with the correct path to the data file on your system.
The path as indicated in the sample file is for data sitting on a local or network drive.

To specify a file in SDMS cloud storage, please use the following notation:
"FileSource": "sd://<path/to/file>/ST10010ZC11_PZ_PSDM_KIRCH_FULL_D.MIG_FIN.POST_STACK.3D.JS-017536.segy"

The following environment variables are required for accessing files in cloud storage:

SD_SVC_URL	URL to SDMS service in the cloud
SD_SVC_API_KEY	API key for accessing SDMS service
SD_SVC_TOKEN	User token for accessing SDMS service

