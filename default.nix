{ pkgs ? import <nixpkgs> { }
, lib ? pkgs.lib
, src ? lib.cleanSource ./.
}:

pkgs.tree-sitter.buildGrammar {
  name = "tree-sitter-moonbit-dev";
  version = "dev";
  inherit src;
  language = "moonbit";

  doCheck = true;
  checkInputs = [
    pkgs.tree-sitter
    pkgs.nodejs
  ];
  checkPhase = ''
    HOME=$(mktemp -d) tree-sitter test
  '';
}
