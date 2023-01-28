package com.example.filedemo.controller;

import com.example.filedemo.exception.FileStorageException;
import com.example.filedemo.payload.UploadFileResponse;
import com.example.filedemo.service.FileStorageService;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.core.io.Resource;
import org.springframework.http.HttpHeaders;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.multipart.MultipartFile;
import org.springframework.web.servlet.support.ServletUriComponentsBuilder;

import javax.servlet.http.HttpServletRequest;
import java.io.IOException;
import java.util.Arrays;
import java.util.List;
import java.util.stream.Collectors;
import java.io.*;
import java.util.*;
import java.lang.*;

@RestController
public class FileController {

    private static final Logger logger = LoggerFactory.getLogger(FileController.class);

    @Autowired
    private FileStorageService fileStorageService;

    @PostMapping("/uploadFile")
    public UploadFileResponse uploadFile(@RequestParam("file") MultipartFile file) {
        String fileName = fileStorageService.storeFile(file);

		int temp=0;
		{
			String[] machine = { "aryastark\n", "saloni\n", "dhruvi\n" };
			try{
				Runtime aRT = Runtime.getRuntime();
				Process aProc = aRT.exec("sh check_connection.sh");
				BufferedReader reader = new BufferedReader(new FileReader("/home/cluster/mpich1/examples/a.txt"));
				String line;
				int tmp, total_pc=3, i=0, active_pc_count=0;
				int pc_status[]=new int[total_pc];
				while((line = reader.readLine()) != null) {
					tmp = Integer.parseInt(line);
					if(tmp == 0) {
						active_pc_count++;
						pc_status[i]=1;
					}
					else {
						pc_status[i]=0;
					}
					i++;
				}
				reader.close();

				FileOutputStream outputStream = new FileOutputStream("/home/cluster/mpich1/share/machines.LINUX");
				for(i=0;i<total_pc;i++) {
					if(pc_status[i] == 1) {
						outputStream.write(machine[i].getBytes());
					}
				}
				outputStream.close();
				temp = active_pc_count;
			} catch (IOException e) {
				System.out.println(e);
			};
			System.out.println("active pc count"+temp);
		}
		Integer temp1 = new Integer(temp);
		// distribute using c
		try {
			 FileWriter fw=new FileWriter("output.txt"); 
	  
		    for (int i = 0; i < fileName.length(); i++) 
		        fw.write(fileName.charAt(i)); 
	  
		    fw.close();

			System.out.println("to c");

			Runtime aRT = Runtime.getRuntime(); //Runtime.getRuntime();
			//Spawn a shell sub-process
			Process aProc = aRT.exec("mpirun -np "+temp1.toString()+" send.out");
			BufferedReader kbdInput = new BufferedReader(new InputStreamReader(aProc.getInputStream()));
			String line;
			while((line = kbdInput.readLine()) !=null)
				System.out.println(line);
		} catch(IOException ex) {
//			System.out.println(ex);
            throw new FileStorageException("Could not distribute file " + fileName + ". Please try again!", ex);
		};
		System.out.println("returned from c");        
		
        String fileDownloadUri = ServletUriComponentsBuilder.fromCurrentContextPath()
                .path("/downloadFile/")
                .path(fileName)
                .toUriString();

        return new UploadFileResponse(fileName, fileDownloadUri,
                file.getContentType(), file.getSize());
    }

    @PostMapping("/uploadMultipleFiles")
    public List<UploadFileResponse> uploadMultipleFiles(@RequestParam("files") MultipartFile[] files) {
        return Arrays.asList(files)
                .stream()
                .map(file -> uploadFile(file))
                .collect(Collectors.toList());
    }

    @GetMapping("/downloadFile/{fileName:.+}")
    public ResponseEntity<Resource> downloadFile(@PathVariable String fileName, HttpServletRequest request) {
    	
    	// check if file uploaded
    	System.out.println("upload check remaining");
		int temp=0;
		{
			String[] machine = { "aryastark\n", "saloni\n", "dhruvi\n" };
			try{
				Runtime aRT = Runtime.getRuntime();
				Process aProc = aRT.exec("sh check_connection.sh");
				BufferedReader reader = new BufferedReader(new FileReader("a.txt"));
				String line;
				int tmp, total_pc=3, i=0, active_pc_count=0;
				int pc_status[]=new int[total_pc];
				while((line = reader.readLine()) != null) {
					tmp = Integer.parseInt(line);
					if(tmp == 0) {
						active_pc_count++;
						pc_status[i]=1;
					}
					else {
						pc_status[i]=0;
					}
					i++;
				}
				reader.close();
				FileOutputStream outputStream = new FileOutputStream("/home/cluster/mpich1/share/machines.LINUX");
				for(i=0;i<total_pc;i++) {
					if(pc_status[i] == 1) {
						outputStream.write(machine[i].getBytes());
					}
				}
				outputStream.close();
				temp = active_pc_count;
			} catch (IOException e) {
				System.out.println(e);
			};
			System.out.println("active pc count"+temp);
		}
		Integer temp1 = new Integer(temp);

    	// get from distributed file
		try {
			 FileWriter fw=new FileWriter("output.txt"); 
	  
		    for (int i = 0; i < fileName.length(); i++) 
		        fw.write(fileName.charAt(i)); 
	  
		    fw.close();
			System.out.println("to c");

			Runtime aRT = Runtime.getRuntime(); //Runtime.getRuntime();
			//Spawn a shell sub-process
			Process aProc = aRT.exec("mpirun -np "+temp1.toString()+" rec.out");
			BufferedReader kbdInput = new BufferedReader(new InputStreamReader(aProc.getInputStream()));
			String line;
			while((line = kbdInput.readLine()) !=null)
				System.out.println(line);
		} catch(IOException ex) {
            throw new FileStorageException("Could not retrive file " + fileName + ". Please try again!", ex);
		};
		System.out.println("returned from c");        

        // Load file as Resource
        Resource resource = fileStorageService.loadFileAsResource(fileName);

        // Try to determine file's content type
        String contentType = null;
        try {
            contentType = request.getServletContext().getMimeType(resource.getFile().getAbsolutePath());
        } catch (IOException ex) {
            logger.info("Could not determine file type.");
        }

        // Fallback to the default content type if type could not be determined
        if(contentType == null) {
            contentType = "application/octet-stream";
        }
//		File file = new File("file/"+fileName);
//		if(file.delete())
//			System.out.println("file deleted successfully");
        return ResponseEntity.ok()
                .contentType(MediaType.parseMediaType(contentType))
                .header(HttpHeaders.CONTENT_DISPOSITION, "attachment; filename=\"" + resource.getFilename() + "\"")
                .body(resource);
    }
}
