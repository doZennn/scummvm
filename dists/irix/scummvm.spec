product scummvm
    id "ScummVM 2.9.0"
    image sw
        id "software"
        version 18
        order 9999
        subsys eoe default
            id "execution only env"
            replaces self
            exp scummvm.sw.eoe
        endsubsys
    endimage
    image man
        id "man pages"
        version 18
        order 9999
        subsys readme default
            id "scummvm documentation"
            replaces self
            exp scummvm.man.readme
        endsubsys
    endimage
endproduct
