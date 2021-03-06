## Amazon S3 - Access Control List representation
## Author: Michal Ludvig <michal@logix.cz>
##         http://www.logix.cz/michal
## License: GPL Version 2

from Utils import *

try:
	import xml.etree.ElementTree as ET
except ImportError:
	import elementtree.ElementTree as ET

class Grantee(object):
	ALL_USERS_URI = "http://acs.amazonaws.com/groups/global/AllUsers"

	def __init__(self):
		self.xsi_type = None
		self.tag = None
		self.name = None
		self.display_name = None
		self.permission = None

	def __repr__(self):
		return 'Grantee("%(tag)s", "%(name)s", "%(permission)s")' % { 
			"tag" : self.tag, 
			"name" : self.name, 
			"permission" : self.permission 
		}

	def isAllUsers(self):
		return self.tag == "URI" and self.name == Grantee.ALL_USERS_URI
	
	def isAnonRead(self):
		return self.isAllUsers and self.permission == "READ"
	
	def getElement(self):
		el = ET.Element("Grant")
		grantee = ET.SubElement(el, "Grantee", { 
			'xmlns:xsi' : 'http://www.w3.org/2001/XMLSchema-instance',
			'xsi:type' : self.xsi_type
		})
		name = ET.SubElement(grantee, self.tag)
		name.text = self.name
		permission = ET.SubElement(el, "Permission")
		permission.text = self.permission
		return el

class GranteeAnonRead(Grantee):
	def __init__(self):
		Grantee.__init__(self)
		self.xsi_type = "Group"
		self.tag = "URI"
		self.name = Grantee.ALL_USERS_URI
		self.permission = "READ"

class ACL(object):
	EMPTY_ACL = "<AccessControlPolicy><Owner><ID></ID></Owner><AccessControlList></AccessControlList></AccessControlPolicy>"

	def __init__(self, xml = None):
		if not xml:
			xml = ACL.EMPTY_ACL

		self.grantees = []
		self.owner_id = ""
		self.owner_nick = ""

		tree = getTreeFromXml(xml)
		self.parseOwner(tree)
		self.parseGrants(tree)

	def parseOwner(self, tree):
		self.owner_id = tree.findtext(".//Owner//ID")
		self.owner_nick = tree.findtext(".//Owner//DisplayName")

	def parseGrants(self, tree):
		for grant in tree.findall(".//Grant"):
			grantee = Grantee()
			g = grant.find(".//Grantee")
			grantee.xsi_type = g.attrib['{http://www.w3.org/2001/XMLSchema-instance}type']
			grantee.permission = grant.find('Permission').text
			for el in g:
				if el.tag == "DisplayName":
					grantee.display_name = el.text
				else:
					grantee.tag = el.tag
					grantee.name = el.text
			self.grantees.append(grantee)

	def getGrantList(self):
		acl = {}
		for grantee in self.grantees:
			if grantee.display_name:
				user = grantee.display_name
			elif grantee.isAllUsers():
				user = "*anon*"
			else:
				user = grantee.name
			acl[user] = grantee.permission
		return acl

	def getOwner(self):
		return { 'id' : self.owner_id, 'nick' : self.owner_nick }

	def isAnonRead(self):
		for grantee in self.grantees:
			if grantee.isAnonRead():
				return True
		return False
	
	def grantAnonRead(self):
		if not self.isAnonRead():
			self.grantees.append(GranteeAnonRead())
	
	def revokeAnonRead(self):
		self.grantees = [g for g in self.grantees if not g.isAnonRead()]

	def __str__(self):
		tree = getTreeFromXml(ACL.EMPTY_ACL)
		tree.attrib['xmlns'] = "http://s3.amazonaws.com/doc/2006-03-01/"
		owner = tree.find(".//Owner//ID")
		owner.text = self.owner_id
		acl = tree.find(".//AccessControlList")
		for grantee in self.grantees:
			acl.append(grantee.getElement())
		return ET.tostring(tree)

if __name__ == "__main__":
	xml = """<?xml version="1.0" encoding="UTF-8"?>
<AccessControlPolicy xmlns="http://s3.amazonaws.com/doc/2006-03-01/">
<Owner>
	<ID>12345678901234567890</ID>
	<DisplayName>owner-nickname</DisplayName>
</Owner>
<AccessControlList>
	<Grant>
		<Grantee xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:type="CanonicalUser">
			<ID>12345678901234567890</ID>
			<DisplayName>owner-nickname</DisplayName>
		</Grantee>
		<Permission>FULL_CONTROL</Permission>
	</Grant>
	<Grant>
		<Grantee xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:type="Group">
			<URI>http://acs.amazonaws.com/groups/global/AllUsers</URI>
		</Grantee>
		<Permission>READ</Permission>
	</Grant>
</AccessControlList>
</AccessControlPolicy>
	"""
	acl = ACL(xml)
	print "Grants:", acl.getGrantList()
	acl.revokeAnonRead()
	print "Grants:", acl.getGrantList()
	acl.grantAnonRead()
	print "Grants:", acl.getGrantList()
	print acl
