use std::fs;

fn main() {
    let data = fs::read("source.txt").expect("Unable to read file");
    
    fs::write("rust_out.txt", data).expect("Unable to write file");
}