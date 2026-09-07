#include <rendering/entity/ItemFrameRenderer.hpp>
#include <entity/ItemFrame.hpp>
#include <rendering/Tesselator.hpp>
#include <rendering/EntityRenderDispatcher.hpp>
#include <rendering/entity/ItemRenderer.hpp>
#include <rendering/TileRenderer.hpp>
#include <item/Item.hpp>
#include <tile/Tile.hpp>
#include <tile/MobHeadTile.hpp>
#include <rendering/tileentity/MobHeadRenderer.hpp>
#include <unigl.h>

ItemFrameRenderer::ItemFrameRenderer()
	: EntityRenderer() {
}

ItemFrameRenderer::~ItemFrameRenderer() {
}

void ItemFrameRenderer::render(Entity* entity, float x, float y, float z, float rot, float a6) {
	ItemFrame* frame = (ItemFrame*)entity;
	if (!frame) return;

	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(frame->yaw, 0.0f, 1.0f, 0.0f);

	GLboolean lightingWas = glIsEnabled(GL_LIGHTING);
	GLboolean cullWas = glIsEnabled(GL_CULL_FACE);
	GLboolean blendWas = glIsEnabled(GL_BLEND);
	GLboolean alphaTestWas = glIsEnabled(GL_ALPHA_TEST);
	GLboolean depthTestWas = glIsEnabled(GL_DEPTH_TEST);

	if (lightingWas) glDisable(GL_LIGHTING);
	if (cullWas) glDisable(GL_CULL_FACE);
	if (!depthTestWas) glEnable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	Tesselator& t = Tesselator::instance;
	this->bindTexture("item_frame.png");

	t.begin(4);
	t.vertexUV(-0.5f, -0.5f, -0.0625f, 0.0f, 1.0f);
	t.vertexUV(0.5f, -0.5f, -0.0625f, 1.0f, 1.0f);
	t.vertexUV(0.5f, 0.5f, -0.0625f, 1.0f, 0.0f);
	t.vertexUV(-0.5f, 0.5f, -0.0625f, 0.0f, 0.0f);

	t.vertexUV(-0.5f, 0.5f, 0.0f, 0.0f, 0.0f);
	t.vertexUV(0.5f, 0.5f, 0.0f, 1.0f, 0.0f);
	t.vertexUV(0.5f, 0.5f, -0.0625f, 1.0f, 0.0625f);
	t.vertexUV(-0.5f, 0.5f, -0.0625f, 0.0f, 0.0625f);

	t.vertexUV(-0.5f, -0.5f, -0.0625f, 0.0f, 0.9375f);
	t.vertexUV(0.5f, -0.5f, -0.0625f, 1.0f, 0.9375f);
	t.vertexUV(0.5f, -0.5f, 0.0f, 1.0f, 1.0f);
	t.vertexUV(-0.5f, -0.5f, 0.0f, 0.0f, 1.0f);

	t.vertexUV(-0.5f, -0.5f, 0.0f, 0.0f, 1.0f);
	t.vertexUV(-0.5f, 0.5f, 0.0f, 0.0f, 0.0f);
	t.vertexUV(-0.5f, 0.5f, -0.0625f, 0.0625f, 0.0f);
	t.vertexUV(-0.5f, -0.5f, -0.0625f, 0.0625f, 1.0f);

	t.vertexUV(0.5f, -0.5f, -0.0625f, 0.9375f, 1.0f);
	t.vertexUV(0.5f, 0.5f, -0.0625f, 0.9375f, 0.0f);
	t.vertexUV(0.5f, 0.5f, 0.0f, 1.0f, 0.0f);
	t.vertexUV(0.5f, -0.5f, 0.0f, 1.0f, 1.0f);

	t.vertexUV(0.5f, -0.5f, 0.0f, 1.0f, 1.0f);
	t.vertexUV(-0.5f, -0.5f, 0.0f, 0.0f, 1.0f);
	t.vertexUV(-0.5f, 0.5f, 0.0f, 0.0f, 0.0f);
	t.vertexUV(0.5f, 0.5f, 0.0f, 1.0f, 0.0f);
	t.draw(1);

	ItemInstance dispItem = frame->getDisplayedItem();
	if (!dispItem.isNull() && dispItem.count > 0) {
		int32_t id = dispItem.getId();
		int32_t aux = dispItem.getAuxValue();

		glPushMatrix();
		glTranslatef(0.0f, 0.0f, -0.07f);
		glRotatef((float)frame->getRotation() * 45.0f, 0.0f, 0.0f, 1.0f);

		float s = 0.28f;
		if (MobHeadTile::isHeadBlock(id)) {
			int htype = MobHeadTile::getHeadType(id);
			if (MobHeadRenderer::instance) {
				glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
				glScalef(0.45f, 0.45f, 0.45f);
				MobHeadRenderer::instance->renderHead(htype, 0.0f, 0.0f, 0.0f, 0.0f, 0.0625f);
			}
		} else if (Tile::enderChest && id == Tile::enderChest->blockID && Item::items[id]) {
			Item* it = Item::items[id];
			this->bindTexture(it->itemTexture.empty() ? "gui/items.png" : it->itemTexture);
			TextureUVCoordinateSet* uvs = it->getIcon(aux, 0, 0);
			if (uvs) {
				t.begin(4);
				t.vertexUV(-s, -s, 0.0f, uvs->minX, uvs->maxY);
				t.vertexUV(s, -s, 0.0f, uvs->maxX, uvs->maxY);
				t.vertexUV(s, s, 0.0f, uvs->maxX, uvs->minY);
				t.vertexUV(-s, s, 0.0f, uvs->minX, uvs->minY);
				t.draw(1);
			}
		} else if (id >= 256 && Item::items[id]) {
			Item* it = Item::items[id];
			this->bindTexture(it->itemTexture.empty() ? "gui/items.png" : it->itemTexture);
			TextureUVCoordinateSet* uvs = it->getIcon(aux, 0, 0);
			if (uvs) {
				t.begin(4);
				t.vertexUV(-s, -s, 0.0f, uvs->minX, uvs->maxY);
				t.vertexUV(s, -s, 0.0f, uvs->maxX, uvs->maxY);
				t.vertexUV(s, s, 0.0f, uvs->maxX, uvs->minY);
				t.vertexUV(-s, s, 0.0f, uvs->minX, uvs->minY);
				t.draw(1);
			}
		} else if (id > 0 && id < 256 && Tile::tiles[id]) {
			this->bindTexture("terrain.png");
			TextureUVCoordinateSet* uvs = Tile::tiles[id]->getTexture(2, aux);
			if (uvs) {
				t.begin(4);
				t.vertexUV(-s, -s, 0.0f, uvs->minX, uvs->maxY);
				t.vertexUV(s, -s, 0.0f, uvs->maxX, uvs->maxY);
				t.vertexUV(s, s, 0.0f, uvs->maxX, uvs->minY);
				t.vertexUV(-s, s, 0.0f, uvs->minX, uvs->minY);
				t.draw(1);
			}
		}
		glPopMatrix();
	}

	if (!alphaTestWas) glDisable(GL_ALPHA_TEST);
	if (!blendWas) glDisable(GL_BLEND);
	if (cullWas) glEnable(GL_CULL_FACE);
	if (lightingWas) glEnable(GL_LIGHTING);
	if (!depthTestWas) glDisable(GL_DEPTH_TEST);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glPopMatrix();
}
