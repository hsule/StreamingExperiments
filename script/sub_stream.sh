RUST_LOG=info zncat --sub -m client -e tcp/140.112.31.242:7447 wtf | ffplay  -fflags nobuffer     -flags low_delay  -framedrop -vf "showinfo" -f mpegts     -

