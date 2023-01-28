package com.example.filedemo.service;

import com.example.filedemo.exception.FileStorageException;
import com.example.filedemo.exception.MyFileNotFoundException;
import com.example.filedemo.property.FileStorageProperties;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.core.io.Resource;
import org.springframework.core.io.UrlResource;
import org.springframework.stereotype.Service;
import org.springframework.util.StringUtils;
import org.springframework.web.multipart.MultipartFile;
import java.io.IOException;
import java.net.MalformedURLException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardCopyOption;
import java.io.*;

@Service
public class FileStorageService {

    private final Path fileStorageLocation;

    @Autowired
    public FileStorageService(FileStorageProperties fileStorageProperties) {
        this.fileStorageLocation = Paths.get(fileStorageProperties.getUploadDir())
                .toAbsolutePath().normalize();

        try {
            Files.createDirectories(this.fileStorageLocation);
        } catch (Exception ex) {
            throw new FileStorageException("Could not create the directory where the uploaded files will be stored.", ex);
        }
    }

    public String storeFile(MultipartFile file) {
        // Normalize file name
        String fileName = StringUtils.cleanPath(file.getOriginalFilename());

        try {
            // Check if the file's name contains invalid characters
            if(fileName.contains("..")) {
                throw new FileStorageException("Sorry! Filename contains invalid path sequence " + fileName);
            }
			// if file exist
			String filename = "/home/cluster/mpich1/examples/allfiles.txt";
			File tempFile = new File("c:/temp/temp.txt");
			boolean exists = tempFile.exists();
			if(exists==true) {
				BufferedReader in = new BufferedReader( new FileReader(filename));
				String str;
				int tmp=0;
				while((str = in.readLine()) != null) {
					System.out.println(str);
					if(str.equals(fileName)==true) {
						tmp=1;
						break;
					}
				}
				if(tmp==1){
					throw new FileStorageException("Sorry! Filename already exists " + fileName);
				}
				in.close();
			}
			appendStrToFile(filename, fileName);
			System.out.println("written ");			
            // Copy file to the target location (Replacing existing file with the same name)
            Path targetLocation = this.fileStorageLocation.resolve(fileName);
            Files.copy(file.getInputStream(), targetLocation, StandardCopyOption.REPLACE_EXISTING);

            return fileName;
        } catch (IOException ex) {
            throw new FileStorageException("Could not store file " + fileName + ". Please try again!", ex);
        }
    }

    public Resource loadFileAsResource(String fileName) {
        try {
            Path filePath = this.fileStorageLocation.resolve(fileName).normalize();
            Resource resource = new UrlResource(filePath.toUri());
            if(resource.exists()) {
                return resource;
            } else {
                throw new MyFileNotFoundException("File not found " + fileName);
            }
        } catch (MalformedURLException ex) {
            throw new MyFileNotFoundException("File not found " + fileName, ex);
        }
    } 
       
    public static void appendStrToFile(String fileName, String str) { 
        try {
            // Open given file in append mode. 
            BufferedWriter out = new BufferedWriter( new FileWriter(fileName, true)); 
            out.write(str+"\n"); 
            out.close(); 
        } 
        catch (IOException ex) { 
            throw new FileStorageException("Could not store file " + fileName + ". Please try again!", ex);
        } 
    } 
}
