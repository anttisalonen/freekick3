#!/usr/bin/env python2
# coding: utf-8

import unittest
import subprocess
import tempfile
import glob
import os
import sys
from lxml import etree
import bz2

class MatchStats:
    def __init__(self):
        self.homegoals = 0
        self.awaygoals = 0

    def fromXML(self, tree):
        root = tree.getroot()
        res = root.xpath("/Match/MatchResult")[0]
        played = int(res.get("played"))
        if not played:
            raise RuntimeError, "Match not played"
        self.homegoals = int(res.get("home"))
        self.awaygoals = int(res.get("away"))

class RunMatches(unittest.TestCase):

    def test_runSkill25(self):
        self.runLeague('skill25')

    def test_runSkill12(self):
        self.runLeague('skill12')

    def test_runSkill1(self):
        self.runLeague('skill1')

    def runMatch(self, datafile, seed, debug = False):
        f = bz2.BZ2File(datafile, 'r') if datafile.lower().endswith('.bz2') else open(datafile, 'r')
        datacontents = f.read()
        f.close()
        tmpfilename = None
        try:
            tmpfile = tempfile.NamedTemporaryFile(delete = False)
            tmpfilename = tmpfile.name
            tmpfile.write(datacontents)
            tmpfile.close()
            if debug:
                print tmpfile.name
                retcode = subprocess.call(['bin/freekick3-match', tmpfile.name, '-o', '-x', '-f', '60', '-s', '%d' % seed])
            else:
                with open(os.devnull, "w") as fnull:
                    retcode = subprocess.call(['bin/freekick3-match', tmpfile.name, '-o', '-x', '-f', '60', '-s', '%d' % seed], stdout = fnull)
            self.assertEqual(retcode, 0)
            result = etree.parse(tmpfile.name)
            ms = MatchStats()
            ms.fromXML(result)
            return ms
        finally:
            if tmpfilename:
                os.remove(tmpfilename)

    def runLeague(self, matchDir):
        allMatchStats = []
        gpm = []
        for seed in [21, 22, 23, 24]:
            sys.stdout.write('%s %d' % (matchDir, seed))
            for datafile in glob.glob('test_cases/%s/*.xml' % matchDir) + glob.glob('test_cases/%s/*.xml.bz2' % matchDir):
                sys.stdout.flush()
                ms = self.runMatch(datafile, seed)
                allMatchStats.append(ms)
                sys.stdout.write(' %d-%d' % (ms.homegoals, ms.awaygoals))
            if allMatchStats:
                goalsPerMatch = sum([s.homegoals + s.awaygoals for s in allMatchStats]) / float(len(allMatchStats))
                print
                print goalsPerMatch, 'goals per match.'
                gpm.append(goalsPerMatch)
        self.checkGPMList(gpm)

    def checkGPMList(self, gpm):
        self.assertTrue(gpm, 'No match results available.')
        minGpm = min(gpm)
        maxGpm = max(gpm)
        avg = sum(gpm) / len(gpm)
        sys.stdout.write('Minimum goals per match: %3.3f\n' % minGpm)
        sys.stdout.write('Maximum goals per match: %3.3f\n' % maxGpm)
        sys.stdout.write('Average goals per match: %3.3f\n' % avg)
        sys.stdout.write('Difference min-max: %3.3f\n' % (maxGpm - minGpm))
        self.assertLessEqual(maxGpm - minGpm, 0.4)
        self.assertGreaterEqual(minGpm, 2.0)
        self.assertLessEqual(maxGpm, 4.0)

if __name__ == '__main__':
    unittest.main()


