import java.util.*;
import java.io.*;
import java.nio.file.Files;
import java.nio.file.Paths;

public class Driver {
    static int pageFaults = 0, tlbHits = 0;
    static byte[] backingStore, physicalMemory = new byte[65536];
    static int[] pageTable = new int[256];
    static HashMap<Integer, Integer> tlb = new HashMap<Integer, Integer>(16);

    public static void main(String[] args) throws IOException {
        backingStore = Files.readAllBytes(Paths.get("BACKING_STORE.bin"));
        for (int i = 0; i < 256; i++) {
            pageTable[i] = -1;
        }

        Scanner addressFile = new Scanner(new File("addresses.txt"));
        int address = 0, addressCount = 0;
        byte value = 0;

        for (; addressFile.hasNext(); addressCount++) {
            address = Integer.parseInt(addressFile.nextLine());

            int pageNumber = getPageNumberFromInt(address);
            int offset = getOffsetFromInt(address);

            value = getByteFromMemory(pageNumber, offset);

            System.out.println("Logical Address: " + address + " Page Number: " + pageNumber + " offset: " + offset
                    + " value: " + value);
        }

        System.out.println(pageFaults + " page faults out of " + addressCount + " look ups means "
                + ((double) pageFaults / addressCount * 100) + " percent of lookups resulted in a page fault");

        System.out.println(tlbHits + " TLB hits out of " + addressCount + " look upds "
                + ((double) tlbHits / addressCount * 100) + " percent of lookups were found in the TLB");
    }

    static private byte getByteFromMemory(int pageNumber, int offset) {
        // System.out.println("Looking up frame for page number: " + pageNumber + " in
        // TLB...");
        Integer frameNumber = null;

        if (tlb.containsKey(pageNumber)) {
            tlbHits++;
            frameNumber = tlb.get(pageNumber);
        }

        if (frameNumber == null) {
            // System.out.println("No results, looking in page table");
            frameNumber = getFrameNumberFromPageTable(pageNumber);

            if (tlb.keySet().size() == 16) {
                tlb.remove(tlb.keySet().toArray()[0]);
                tlb.put(pageNumber, frameNumber);
            } else {
                tlb.put(pageNumber, frameNumber);
            }
        }

        // System.out.println("Getting value for frame " + frameNumber.intValue() + "
        // and offset " + offset);
        return getByteFromPhysicalMemory(frameNumber.intValue(), offset);
    }

    static private Integer getFrameNumberFromPageTable(int pageNumber) {
        if (pageTable[pageNumber] != -1) {
            // System.out.println("Found in page table");
            return pageTable[pageNumber];
        }

        // We only have on process that has access to all of physical memory, the page
        // numbers and frame numbers will match.
        // System.out.println("No result in page table, retrieving from backing
        // store...");
        pageTable[pageNumber] = pageNumber;

        loadPageFromBackingStoreIntoPhysicalMemory(pageNumber);

        pageFaults++;

        return pageNumber;
    }

    static private void loadPageFromBackingStoreIntoPhysicalMemory(int pageNumber) {
        int start = pageNumber * 256;
        int end = start + 255;

        // System.out.println("Loading values from backing store from " + start + " to "
        // + end);
        for (int i = start; i <= end; i++) {
            physicalMemory[i] = backingStore[i];
        }
    }

    static private byte getByteFromPhysicalMemory(int frameNumber, int offset) {
        return physicalMemory[(frameNumber * 256) + offset];
    }

    static private int getPageNumberFromInt(int address) {
        int pageNumberMask = Integer.parseUnsignedInt("00000000000000001111111100000000", 2);

        return (address & pageNumberMask) >> 8;
    }

    static private int getOffsetFromInt(int address) {
        int offsetMask = Integer.parseUnsignedInt("00000000000000000000000011111111", 2);

        return address & offsetMask;
    }
}
