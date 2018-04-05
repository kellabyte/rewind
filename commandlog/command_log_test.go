package commandlog

import (
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestNew(t *testing.T) {
	log := New()
	assert.NotNil(t, log)
}
