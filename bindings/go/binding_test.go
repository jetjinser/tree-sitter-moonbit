package tree_sitter_moonbit_test

import (
	"testing"

	tree_sitter "github.com/smacker/go-tree-sitter"
	"github.com/tree-sitter/tree-sitter-moonbit"
)

func TestCanLoadGrammar(t *testing.T) {
	language := tree_sitter.NewLanguage(tree_sitter_moonbit.Language())
	if language == nil {
		t.Errorf("Error loading Moonbit grammar")
	}
}
