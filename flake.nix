{
  description = "Filetransfer client app for modded Corgi3DS";
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    devkitnix = {
      url = "github:mrsmoer/devkitnix";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = {
    self,
    nixpkgs,
    flake-utils,
    devkitnix,
  }: 
    flake-utils.lib.eachDefaultSystem(system:
    let
      name = "Corgi3DS-filetransfer";
      pkgs = import nixpkgs {inherit system; };
      devkitARM = devkitnix.packages.${system}.devkitARM;
      buildInputs = [ devkitARM ];
      src = self;
    in {
      
      packages.corgi3ds-filetransfer = pkgs.stdenv.mkDerivation {
        inherit buildInputs name src;
        installPhase = ''
          mkdir $out
          cp source.3dsx $out/corgi3ds-filetransfer.3dsx
          cp source.elf $out/corgi3ds-filetransfer.elf
        '';
      };

      packages.default = self.packages.${system}.corgi3ds-filetransfer;

      devShells.default = pkgs.mkShell {
        inherit buildInputs name;
      };
  });
}
