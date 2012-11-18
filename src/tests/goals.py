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
import collections

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

    def runMatch(self, datafile, seed, numFPS, debug = False):
        f = bz2.BZ2File(datafile, 'r') if datafile.lower().endswith('.bz2') else open(datafile, 'r')
        datacontents = f.read()
        f.close()
        tmpfilename = None
        try:
            tmpfile = tempfile.NamedTemporaryFile(delete = False)
            tmpfilename = tmpfile.name
            tmpfile.write(datacontents)
            tmpfile.close()
            cmd = ['bin/freekick3-match', tmpfile.name, '-o', '-x', '-f', str(numFPS), '-s', str(seed)]
            if debug:
                print tmpfile.name
                print cmd
                retcode = subprocess.call(cmd)
            else:
                with open(os.devnull, "w") as fnull:
                    retcode = subprocess.call(cmd, stdout = fnull)
            self.assertEqual(retcode, 0)
            result = etree.parse(tmpfile.name)
            ms = MatchStats()
            ms.fromXML(result)
            return ms
        finally:
            if tmpfilename:
                os.remove(tmpfilename)

    def runLeagues(self, leagues, numSeeds = 4, maxNumMatches = None, numFPS = 60):
        gpm = collections.defaultdict(list)
        for seed in xrange(21, 21 + numSeeds):
            allMatchStats = []
            for matchDir in leagues:
                sys.stdout.write('%s %d' % (matchDir, seed))
                allMatches = sorted(glob.glob('test_cases/%s/*.xml' % matchDir) + glob.glob('test_cases/%s/*.xml.bz2' % matchDir))
                if maxNumMatches:
                    allMatches = allMatches[:maxNumMatches]
                i = 0
                for datafile in allMatches:
                    sys.stdout.flush()
                    ms = self.runMatch(datafile, seed, numFPS)
                    allMatchStats.append(ms)
                    sys.stdout.write(' %d-%d' % (ms.homegoals, ms.awaygoals))
                    i += 1
                    if i % 10 == 0:
                        self.printGpmStats(allMatchStats)
                if allMatchStats:
                    goalsPerMatch = self.printGpmStats(allMatchStats)
                    gpm[matchDir].append(goalsPerMatch)
        for matchDir in leagues:
            self.checkGPMList(gpm[matchDir])
 
    def runLeague(self, matchDir, numSeeds = 4, numFPS = 60):
        self.runLeagues([matchDir], numSeeds, None, numFPS)

    def printGpmStats(self, allMatchStats):
        goalsPerMatch = sum([s.homegoals + s.awaygoals for s in allMatchStats]) / float(len(allMatchStats))
        print
        print goalsPerMatch, 'goals per match.'
        return goalsPerMatch

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

    def test_Smoke(self):
        self.runLeagues(['skill1', 'skill12', 'skill25'], 2, None, 30)

    def test_Full(self):
        self.runLeagues(['skill1', 'skill12', 'skill25'])

if __name__ == '__main__':
    unittest.main()


